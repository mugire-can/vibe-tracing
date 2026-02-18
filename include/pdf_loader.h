/*
 * pdf_loader.h - Simple PDF file loading library
 *
 * Provides basic functionality to open, inspect, and extract text
 * content from PDF files.
 */

#ifndef PDF_LOADER_H
#define PDF_LOADER_H

#include <stddef.h>
#include <stdio.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Error codes */
typedef enum {
    PDF_OK = 0,
    PDF_ERR_FILE_NOT_FOUND,
    PDF_ERR_INVALID_FORMAT,
    PDF_ERR_OUT_OF_MEMORY,
    PDF_ERR_READ_FAILED,
    PDF_ERR_INVALID_ARGUMENT
} pdf_error_t;

/* Opaque handle for a loaded PDF document */
typedef struct pdf_document pdf_document_t;

/*
 * pdf_open - Open a PDF file and load its structure.
 *
 * @path:     Path to the PDF file.
 * @doc_out:  On success, receives a pointer to the loaded document.
 *
 * Returns PDF_OK on success or an error code on failure.
 */
pdf_error_t pdf_open(const char *path, pdf_document_t **doc_out);

/*
 * pdf_open_from_memory - Load a PDF from a memory buffer.
 *
 * @data:     Pointer to the PDF data.
 * @size:     Size of the data in bytes.
 * @doc_out:  On success, receives a pointer to the loaded document.
 *
 * Returns PDF_OK on success or an error code on failure.
 */
pdf_error_t pdf_open_from_memory(const unsigned char *data, size_t size,
                                  pdf_document_t **doc_out);

/*
 * pdf_page_count - Get the number of pages in the document.
 *
 * @doc: A loaded PDF document.
 *
 * Returns the page count, or 0 if doc is NULL.
 */
int pdf_page_count(const pdf_document_t *doc);

/*
 * pdf_get_version - Get the PDF version string (e.g. "1.4").
 *
 * @doc: A loaded PDF document.
 *
 * Returns a pointer to the version string, or NULL if doc is NULL.
 * The string is owned by the document and must not be freed.
 */
const char *pdf_get_version(const pdf_document_t *doc);

/*
 * pdf_extract_text - Extract text content from a specific page.
 *
 * @doc:       A loaded PDF document.
 * @page:      Zero-based page index.
 * @text_out:  On success, receives a newly allocated string with the
 *             extracted text. Caller must free() this string.
 * @len_out:   On success, receives the length of the text.
 *
 * Returns PDF_OK on success or an error code on failure.
 */
pdf_error_t pdf_extract_text(const pdf_document_t *doc, int page,
                              char **text_out, size_t *len_out);

/*
 * pdf_get_file_size - Get the size of the loaded PDF in bytes.
 *
 * @doc: A loaded PDF document.
 *
 * Returns the file size, or 0 if doc is NULL.
 */
size_t pdf_get_file_size(const pdf_document_t *doc);

/*
 * pdf_error_string - Get a human-readable description of an error code.
 *
 * @err: An error code returned by one of the library functions.
 *
 * Returns a static string describing the error.
 */
const char *pdf_error_string(pdf_error_t err);

/*
 * pdf_close - Free all resources associated with a loaded PDF document.
 *
 * @doc: The document to close. May be NULL (no-op).
 */
void pdf_close(pdf_document_t *doc);

#ifdef __cplusplus
}
#endif

#endif /* PDF_LOADER_H */
