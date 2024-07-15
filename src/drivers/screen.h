//
// screen.h
//
// screen utils header
//
// prefix: screen_ (e.g. screen_function_name)
//
#ifndef SCREEN_H
    #define SCREEN_H

    #include "llio.h"

    #define SCREEN_VIDEO_ADDRESS 0xB8000
    #define SCREEN_MAX_ROWS 25
    #define SCREEN_MAX_COLS 80
    #define WHITE_ON_BLACK 0x0F
    // screen device io ports
    #define REG_SCREEN_CTRL 0x3D4
    #define REG_SCREEN_DATA 0x3D5

    // returns the calculated memory offset from SCREEN_VIDEO_ADDRESS
    int screen_get_offset(int col, int row) {
        return (row * SCREEN_MAX_COLS + col) << 1;
    }

    int screen_get_cursor_offset() {
	// The device uses its control register as an index to 
	// select its internal registers, of which we are interested in:
	//
	//     reg 14: high byte of cursor's offset
	//     reg 15: low  byte of cursor's offset
	//
	// Once the internal register has been selected, we may read or
	// write a byte on the data register.
        llio_port_byte_out(REG_SCREEN_CTRL, 14);
	int offset = llio_port_byte_in(REG_SCREEN_DATA) << 8;
	llio_port_byte_out(REG_SCREEN_CTRL, 15);
	offset += llio_port_byte_in(REG_SCREEN_DATA);

	// Since the cursor offset reported by the VGA hardware is the
	// number of characters, we multiply by two to convert it to
	// a character cell offset
	return offset << 1;
    }

    void screen_set_cursor(int offset) {
	// convert the cell offset to the char offset
        offset /= 2;
	// similar to get_cursor, only now we write
	// bytes to those internal device registers.
	llio_port_byte_out(REG_SCREEN_CTRL, 14);
	llio_port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset >> 8));
        llio_port_byte_out(REG_SCREEN_CTRL, 15);
	llio_port_byte_out(REG_SCREEN_DATA, (unsigned char)(offset & 0xFF));
    }

    // scrolls the screen when the cursor is outside the screen
    // and returns the new offset of the cursor
    int screen_handle_scrolling(int offset) {
        return offset; // TODO for now...
    }

    // print a char on the screen at row, col or at the cursor position
    // default values:
    //     col = -1: current cursor position
    //     row = -1: current cursor position
    //     attribute_byte = 0: white char on black background
    int screen_print_char(char character, int col, int row, char attribute_byte) {
        unsigned char* video_memory = (unsigned char*)SCREEN_VIDEO_ADDRESS;
        // if attribute byte is zero, assume default style
        if (attribute_byte == 0) {
            attribute_byte = WHITE_ON_BLACK;
        }

        int offset;
        if (col >= 0 && row >= 0) {
            offset = screen_get_offset(col, row);
        } else {
            offset = screen_get_cursor_offset();
        }

        if (character == '\n') {
            row = offset / (SCREEN_MAX_COLS << 1);
	    offset = screen_get_offset(0, row + 1);
        } else {
            video_memory[offset] = character;
	    video_memory[offset + 1] = attribute_byte;
	    offset += 2;
        }

        offset = screen_handle_scrolling(offset);
        screen_set_cursor(offset);
	return offset;
    }

    int screen_get_row(int offset) {
        return offset / (2 * SCREEN_MAX_COLS);
    }

    int screen_get_col(int offset) {
        return (offset - (screen_get_row(offset) * 2 * SCREEN_MAX_COLS)) / 2;
    }

    int screen_get_col_with_row(int offset, int row) {
        return (offset - (row * 2 * SCREEN_MAX_COLS)) / 2;
    }

    void screen_print_at(char* msg, int col, int row) {
        int offset;
	if (col >= 0 && row >= 0) {
            offset = screen_get_offset(col, row);
	} else {
            offset = screen_get_cursor_offset();
	    row = screen_get_row(offset);
	    col = screen_get_col_with_row(offset, row);
	}
	
        int i = 0;
	while (msg[i] != 0) {
            offset = screen_print_char(msg[i++], col, row, WHITE_ON_BLACK);
	    row = screen_get_row(offset);
	    col = screen_get_col(offset);
	}
    }

    void screen_print(char* message) {
	screen_print_at(message, -1, -1);
    }

    void screen_clear() {
	int max = (SCREEN_MAX_ROWS * SCREEN_MAX_COLS) << 1;
        for (int offset = 0; offset < max; offset += 2) {
            ((unsigned char*)(SCREEN_VIDEO_ADDRESS))[offset] = ' ';
	    ((unsigned char*)(SCREEN_VIDEO_ADDRESS))[offset + 1] = WHITE_ON_BLACK;
	}
	screen_set_cursor(0);
    }

#endif
