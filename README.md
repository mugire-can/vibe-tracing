# vibe-tracing

A lightweight C library for loading and inspecting PDF files.

## Features

- Open PDF files from disk or from a memory buffer
- Validate PDF structure (header, cross-reference, EOF marker)
- Query document metadata (version, page count, file size)
- Basic text extraction from PDF streams
- Clean error handling with descriptive error codes

## Building

```sh
make          # build library and example
make test     # build and run tests
make clean    # remove build artifacts
```

## Usage

```c
#include "pdf_loader.h"

pdf_document_t *doc = NULL;
pdf_error_t err = pdf_open("document.pdf", &doc);
if (err != PDF_OK) {
    fprintf(stderr, "Error: %s\n", pdf_error_string(err));
    return 1;
}

printf("Version: %s\n", pdf_get_version(doc));
printf("Pages:   %d\n", pdf_page_count(doc));

char *text = NULL;
size_t len = 0;
pdf_extract_text(doc, 0, &text, &len);
printf("Text: %s\n", text);
free(text);

pdf_close(doc);
```

## API

| Function | Description |
|---|---|
| `pdf_open` | Open a PDF file from disk |
| `pdf_open_from_memory` | Load a PDF from a memory buffer |
| `pdf_page_count` | Get the number of pages |
| `pdf_get_version` | Get the PDF version string |
| `pdf_get_file_size` | Get the file size in bytes |
| `pdf_extract_text` | Extract text from a specific page |
| `pdf_error_string` | Get a human-readable error description |
| `pdf_close` | Free all resources |

## Project Structure

```
include/pdf_loader.h   - Public API header
src/pdf_loader.c       - Library implementation
examples/load_pdf.c    - Example: load and inspect a PDF
tests/test_pdf_loader.c - Unit tests
Makefile               - Build system
```