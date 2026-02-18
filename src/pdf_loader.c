/*
 * pdf_loader.c - Simple PDF file loading library implementation
 *
 * Parses the basic structure of a PDF file: validates the header,
 * locates the cross-reference table, counts pages, and extracts
 * stream text content.
 */

#include "pdf_loader.h"

#include <stdlib.h>
#include <string.h>

/* Internal document structure */
struct pdf_document {
    unsigned char *data;   /* Raw PDF bytes (owned)               */
    size_t         size;   /* Total byte count                    */
    char           version[8]; /* PDF version string, e.g. "1.4" */
    int            page_count; /* Number of pages discovered      */
};

/* ------------------------------------------------------------------ */
/*  Helper: search backwards for a keyword starting from the end      */
/* ------------------------------------------------------------------ */
static const unsigned char *find_last(const unsigned char *data, size_t size,
                                      const char *keyword)
{
    size_t klen = strlen(keyword);
    if (klen > size)
        return NULL;

    for (size_t i = size - klen; i > 0; i--) {
        if (memcmp(data + i, keyword, klen) == 0)
            return data + i;
    }
    if (memcmp(data, keyword, klen) == 0)
        return data;
    return NULL;
}

/* ------------------------------------------------------------------ */
/*  Helper: count occurrences of "/Type /Page" (one per page object)  */
/* ------------------------------------------------------------------ */
static int count_pages(const unsigned char *data, size_t size)
{
    const char *needle = "/Type /Page";
    const char *pages_needle = "/Type /Pages";
    size_t nlen = strlen(needle);
    size_t plen = strlen(pages_needle);
    int count = 0;

    for (size_t i = 0; i + nlen <= size; i++) {
        if (memcmp(data + i, needle, nlen) == 0) {
            /* Make sure this is not "/Type /Pages" */
            if (i + plen <= size && memcmp(data + i, pages_needle, plen) == 0)
                continue;
            count++;
        }
    }
    return count > 0 ? count : 1; /* At least 1 page if the file is valid */
}

/* ------------------------------------------------------------------ */
/*  Helper: parse the PDF header to extract version                   */
/* ------------------------------------------------------------------ */
static pdf_error_t parse_header(struct pdf_document *doc)
{
    if (doc->size < 8)
        return PDF_ERR_INVALID_FORMAT;

    /* PDF files must start with %PDF-x.y */
    if (memcmp(doc->data, "%PDF-", 5) != 0)
        return PDF_ERR_INVALID_FORMAT;

    /* Extract version string (e.g. "1.4", "2.0") */
    size_t vlen = 0;
    for (size_t i = 5; i < doc->size && vlen < sizeof(doc->version) - 1; i++) {
        char c = (char)doc->data[i];
        if (c == '\r' || c == '\n' || c == ' ')
            break;
        doc->version[vlen++] = c;
    }
    doc->version[vlen] = '\0';

    return PDF_OK;
}

/* ------------------------------------------------------------------ */
/*  Helper: verify cross-reference / trailer presence                 */
/* ------------------------------------------------------------------ */
static pdf_error_t validate_structure(const struct pdf_document *doc)
{
    /* A valid PDF should have %%EOF near the end */
    if (!find_last(doc->data, doc->size, "%%EOF"))
        return PDF_ERR_INVALID_FORMAT;

    return PDF_OK;
}

/* ------------------------------------------------------------------ */
/*  Helper: build internal document from raw buffer                   */
/* ------------------------------------------------------------------ */
static pdf_error_t build_document(unsigned char *data, size_t size,
                                  pdf_document_t **doc_out)
{
    struct pdf_document *doc = calloc(1, sizeof(*doc));
    if (!doc) {
        free(data);
        return PDF_ERR_OUT_OF_MEMORY;
    }

    doc->data = data;
    doc->size = size;

    pdf_error_t err = parse_header(doc);
    if (err != PDF_OK) {
        pdf_close(doc);
        return err;
    }

    err = validate_structure(doc);
    if (err != PDF_OK) {
        pdf_close(doc);
        return err;
    }

    doc->page_count = count_pages(doc->data, doc->size);

    *doc_out = doc;
    return PDF_OK;
}

/* ------------------------------------------------------------------ */
/*  Public API                                                        */
/* ------------------------------------------------------------------ */

pdf_error_t pdf_open(const char *path, pdf_document_t **doc_out)
{
    if (!path || !doc_out)
        return PDF_ERR_INVALID_ARGUMENT;

    FILE *fp = fopen(path, "rb");
    if (!fp)
        return PDF_ERR_FILE_NOT_FOUND;

    /* Determine file size */
    if (fseek(fp, 0, SEEK_END) != 0) {
        fclose(fp);
        return PDF_ERR_READ_FAILED;
    }
    long len = ftell(fp);
    if (len <= 0) {
        fclose(fp);
        return PDF_ERR_READ_FAILED;
    }
    rewind(fp);

    size_t size = (size_t)len;
    unsigned char *buf = malloc(size);
    if (!buf) {
        fclose(fp);
        return PDF_ERR_OUT_OF_MEMORY;
    }

    if (fread(buf, 1, size, fp) != size) {
        free(buf);
        fclose(fp);
        return PDF_ERR_READ_FAILED;
    }
    fclose(fp);

    return build_document(buf, size, doc_out);
}

pdf_error_t pdf_open_from_memory(const unsigned char *data, size_t size,
                                  pdf_document_t **doc_out)
{
    if (!data || size == 0 || !doc_out)
        return PDF_ERR_INVALID_ARGUMENT;

    unsigned char *copy = malloc(size);
    if (!copy)
        return PDF_ERR_OUT_OF_MEMORY;

    memcpy(copy, data, size);
    return build_document(copy, size, doc_out);
}

int pdf_page_count(const pdf_document_t *doc)
{
    return doc ? doc->page_count : 0;
}

const char *pdf_get_version(const pdf_document_t *doc)
{
    return doc ? doc->version : NULL;
}

size_t pdf_get_file_size(const pdf_document_t *doc)
{
    return doc ? doc->size : 0;
}

/*
 * Simple text extraction: scans for stream..endstream pairs and
 * collects printable ASCII content.  This is intentionally basic—
 * full PDF text extraction would require decoding filters (FlateDecode,
 * etc.) and interpreting text-showing operators (Tj, TJ, etc.).
 */
pdf_error_t pdf_extract_text(const pdf_document_t *doc, int page,
                              char **text_out, size_t *len_out)
{
    if (!doc || !text_out || !len_out)
        return PDF_ERR_INVALID_ARGUMENT;
    if (page < 0 || page >= doc->page_count)
        return PDF_ERR_INVALID_ARGUMENT;

    const char *stream_tag = "stream";
    const char *endstream_tag = "endstream";
    size_t slen = strlen(stream_tag);
    size_t elen = strlen(endstream_tag);

    /* Collect printable text from all streams */
    size_t cap = 4096;
    size_t pos = 0;
    char *text = malloc(cap);
    if (!text)
        return PDF_ERR_OUT_OF_MEMORY;

    int streams_found = 0;

    for (size_t i = 0; i + slen < doc->size; i++) {
        if (memcmp(doc->data + i, stream_tag, slen) != 0)
            continue;
        /* Make sure it's not "endstream" */
        if (i >= elen && memcmp(doc->data + i - 3, "end", 3) == 0)
            continue;

        /* Skip past "stream" and the newline */
        size_t start = i + slen;
        if (start < doc->size && doc->data[start] == '\r')
            start++;
        if (start < doc->size && doc->data[start] == '\n')
            start++;

        /* Find matching "endstream" */
        const unsigned char *end = NULL;
        for (size_t j = start; j + elen <= doc->size; j++) {
            if (memcmp(doc->data + j, endstream_tag, elen) == 0) {
                end = doc->data + j;
                break;
            }
        }
        if (!end)
            continue;

        /* Skip streams until we reach the desired page (rough heuristic) */
        if (streams_found > 0 && streams_found <= page) {
            streams_found++;
            i = (size_t)(end - doc->data) + elen - 1;
            continue;
        }

        /* Copy printable ASCII from the stream */
        for (const unsigned char *p = doc->data + start; p < end; p++) {
            if (*p >= 32 && *p < 127) {
                if (pos + 1 >= cap) {
                    cap *= 2;
                    char *tmp = realloc(text, cap);
                    if (!tmp) {
                        free(text);
                        return PDF_ERR_OUT_OF_MEMORY;
                    }
                    text = tmp;
                }
                text[pos++] = (char)*p;
            }
        }

        streams_found++;
        if (streams_found > page + 1)
            break;

        i = (size_t)(end - doc->data) + elen - 1;
    }

    text[pos] = '\0';
    *text_out = text;
    *len_out = pos;
    return PDF_OK;
}

const char *pdf_error_string(pdf_error_t err)
{
    switch (err) {
    case PDF_OK:                  return "Success";
    case PDF_ERR_FILE_NOT_FOUND:  return "File not found";
    case PDF_ERR_INVALID_FORMAT:  return "Invalid PDF format";
    case PDF_ERR_OUT_OF_MEMORY:   return "Out of memory";
    case PDF_ERR_READ_FAILED:     return "Read failed";
    case PDF_ERR_INVALID_ARGUMENT: return "Invalid argument";
    default:                       return "Unknown error";
    }
}

void pdf_close(pdf_document_t *doc)
{
    if (!doc)
        return;
    free(doc->data);
    free(doc);
}
