/*
 * load_pdf.c - Example: load a PDF and print basic information
 *
 * Usage: ./load_pdf <file.pdf>
 */

#include <stdio.h>
#include <stdlib.h>

#include "pdf_loader.h"

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <file.pdf>\n", argv[0]);
        return 1;
    }

    pdf_document_t *doc = NULL;
    pdf_error_t err = pdf_open(argv[1], &doc);
    if (err != PDF_OK) {
        fprintf(stderr, "Error opening '%s': %s\n", argv[1],
                pdf_error_string(err));
        return 1;
    }

    printf("File:      %s\n", argv[1]);
    printf("Size:      %zu bytes\n", pdf_get_file_size(doc));
    printf("Version:   %s\n", pdf_get_version(doc));
    printf("Pages:     %d\n", pdf_page_count(doc));
    printf("\n");

    /* Extract and print text from each page */
    for (int i = 0; i < pdf_page_count(doc); i++) {
        char *text = NULL;
        size_t len = 0;

        err = pdf_extract_text(doc, i, &text, &len);
        if (err != PDF_OK) {
            fprintf(stderr, "Error extracting page %d: %s\n", i,
                    pdf_error_string(err));
            continue;
        }

        printf("--- Page %d (%zu chars) ---\n", i + 1, len);
        if (len > 0)
            printf("%s\n", text);
        else
            printf("(no extractable text)\n");
        printf("\n");

        free(text);
    }

    pdf_close(doc);
    return 0;
}
