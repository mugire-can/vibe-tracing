/*
 * test_pdf_loader.c - Unit tests for the PDF loader library
 *
 * A minimal test harness that exercises the public API.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pdf_loader.h"

static int tests_run    = 0;
static int tests_passed = 0;

#define ASSERT(cond, msg)                                           \
    do {                                                            \
        tests_run++;                                                \
        if (!(cond)) {                                              \
            fprintf(stderr, "  FAIL: %s (line %d)\n", msg, __LINE__); \
        } else {                                                    \
            tests_passed++;                                         \
            fprintf(stderr, "  PASS: %s\n", msg);                   \
        }                                                           \
    } while (0)

/* ---- Minimal valid PDF (no real content, but structurally OK) ---- */
static const char MINIMAL_PDF[] =
    "%PDF-1.4\n"
    "1 0 obj\n"
    "<< /Type /Catalog /Pages 2 0 R >>\n"
    "endobj\n"
    "2 0 obj\n"
    "<< /Type /Pages /Kids [3 0 R] /Count 1 >>\n"
    "endobj\n"
    "3 0 obj\n"
    "<< /Type /Page /Parent 2 0 R /MediaBox [0 0 612 792] >>\n"
    "endobj\n"
    "xref\n"
    "0 4\n"
    "trailer\n"
    "<< /Root 1 0 R >>\n"
    "startxref\n"
    "0\n"
    "%%EOF\n";

/* ---- PDF with a simple text stream ---- */
static const char PDF_WITH_TEXT[] =
    "%PDF-1.4\n"
    "1 0 obj\n"
    "<< /Type /Catalog /Pages 2 0 R >>\n"
    "endobj\n"
    "2 0 obj\n"
    "<< /Type /Pages /Kids [3 0 R] /Count 1 >>\n"
    "endobj\n"
    "3 0 obj\n"
    "<< /Type /Page /Parent 2 0 R /MediaBox [0 0 612 792]"
    " /Contents 4 0 R >>\n"
    "endobj\n"
    "4 0 obj\n"
    "<< /Length 44 >>\n"
    "stream\n"
    "BT /F1 12 Tf 100 700 Td (Hello PDF) Tj ET\n"
    "endstream\n"
    "endobj\n"
    "xref\n"
    "0 5\n"
    "trailer\n"
    "<< /Root 1 0 R >>\n"
    "startxref\n"
    "0\n"
    "%%EOF\n";

/* ------------------------------------------------------------------ */
static void test_null_arguments(void)
{
    fprintf(stderr, "\n[test_null_arguments]\n");

    pdf_error_t err;
    pdf_document_t *doc = NULL;

    err = pdf_open(NULL, &doc);
    ASSERT(err == PDF_ERR_INVALID_ARGUMENT, "pdf_open(NULL path)");

    err = pdf_open("nonexistent.pdf", NULL);
    ASSERT(err == PDF_ERR_INVALID_ARGUMENT, "pdf_open(NULL doc_out)");

    err = pdf_open_from_memory(NULL, 100, &doc);
    ASSERT(err == PDF_ERR_INVALID_ARGUMENT, "pdf_open_from_memory(NULL data)");

    err = pdf_open_from_memory((const unsigned char *)"x", 0, &doc);
    ASSERT(err == PDF_ERR_INVALID_ARGUMENT, "pdf_open_from_memory(size 0)");

    ASSERT(pdf_page_count(NULL) == 0, "pdf_page_count(NULL)");
    ASSERT(pdf_get_version(NULL) == NULL, "pdf_get_version(NULL)");
    ASSERT(pdf_get_file_size(NULL) == 0, "pdf_get_file_size(NULL)");

    /* pdf_close(NULL) should be a no-op */
    pdf_close(NULL);
    ASSERT(1, "pdf_close(NULL) no crash");
}

static void test_invalid_format(void)
{
    fprintf(stderr, "\n[test_invalid_format]\n");

    pdf_document_t *doc = NULL;
    const char *garbage = "This is not a PDF file at all.\n";

    pdf_error_t err = pdf_open_from_memory(
        (const unsigned char *)garbage, strlen(garbage), &doc);
    ASSERT(err == PDF_ERR_INVALID_FORMAT, "reject non-PDF data");
    ASSERT(doc == NULL, "doc remains NULL after rejection");
}

static void test_minimal_pdf(void)
{
    fprintf(stderr, "\n[test_minimal_pdf]\n");

    pdf_document_t *doc = NULL;
    pdf_error_t err = pdf_open_from_memory(
        (const unsigned char *)MINIMAL_PDF, strlen(MINIMAL_PDF), &doc);

    ASSERT(err == PDF_OK, "open minimal PDF");
    ASSERT(doc != NULL, "doc is not NULL");

    ASSERT(strcmp(pdf_get_version(doc), "1.4") == 0, "version is 1.4");
    ASSERT(pdf_page_count(doc) >= 1, "at least 1 page");
    ASSERT(pdf_get_file_size(doc) == strlen(MINIMAL_PDF), "file size matches");

    pdf_close(doc);
}

static void test_text_extraction(void)
{
    fprintf(stderr, "\n[test_text_extraction]\n");

    pdf_document_t *doc = NULL;
    pdf_error_t err = pdf_open_from_memory(
        (const unsigned char *)PDF_WITH_TEXT, strlen(PDF_WITH_TEXT), &doc);

    ASSERT(err == PDF_OK, "open PDF with text");

    char *text = NULL;
    size_t len = 0;
    err = pdf_extract_text(doc, 0, &text, &len);
    ASSERT(err == PDF_OK, "extract text from page 0");
    ASSERT(text != NULL, "text is not NULL");
    ASSERT(len > 0, "text length > 0");

    /* The stream contains "Hello PDF" as printable text */
    ASSERT(strstr(text, "Hello PDF") != NULL, "extracted text contains 'Hello PDF'");

    free(text);

    /* Out-of-range page */
    err = pdf_extract_text(doc, 999, &text, &len);
    ASSERT(err == PDF_ERR_INVALID_ARGUMENT, "reject out-of-range page");

    pdf_close(doc);
}

static void test_file_not_found(void)
{
    fprintf(stderr, "\n[test_file_not_found]\n");

    pdf_document_t *doc = NULL;
    pdf_error_t err = pdf_open("/nonexistent/path/file.pdf", &doc);
    ASSERT(err == PDF_ERR_FILE_NOT_FOUND, "open nonexistent file");
}

static void test_error_strings(void)
{
    fprintf(stderr, "\n[test_error_strings]\n");

    ASSERT(strcmp(pdf_error_string(PDF_OK), "Success") == 0,
           "PDF_OK string");
    ASSERT(strcmp(pdf_error_string(PDF_ERR_FILE_NOT_FOUND), "File not found") == 0,
           "PDF_ERR_FILE_NOT_FOUND string");
    ASSERT(strcmp(pdf_error_string(PDF_ERR_INVALID_FORMAT), "Invalid PDF format") == 0,
           "PDF_ERR_INVALID_FORMAT string");
    ASSERT(strcmp(pdf_error_string(PDF_ERR_OUT_OF_MEMORY), "Out of memory") == 0,
           "PDF_ERR_OUT_OF_MEMORY string");
    ASSERT(strcmp(pdf_error_string(PDF_ERR_READ_FAILED), "Read failed") == 0,
           "PDF_ERR_READ_FAILED string");
    ASSERT(strcmp(pdf_error_string(PDF_ERR_INVALID_ARGUMENT), "Invalid argument") == 0,
           "PDF_ERR_INVALID_ARGUMENT string");
    ASSERT(strlen(pdf_error_string((pdf_error_t)999)) > 0,
           "unknown error has non-empty string");
}

/* ------------------------------------------------------------------ */
int main(void)
{
    test_null_arguments();
    test_invalid_format();
    test_minimal_pdf();
    test_text_extraction();
    test_file_not_found();
    test_error_strings();

    fprintf(stderr, "\n========================================\n");
    fprintf(stderr, "Results: %d / %d passed\n", tests_passed, tests_run);
    fprintf(stderr, "========================================\n");

    return (tests_passed == tests_run) ? 0 : 1;
}
