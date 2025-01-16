#ifndef BITMAP_H
#define BITMAP_H

#ifndef CHAR_BIT
#define CHAR_BIT 8
#endif /*CHAR_BIT*/

#define word_offset(bit) ((bit) / CHAR_BIT)
#define bit_offset(bit)  ((bit) % CHAR_BIT)

#define Bitmap(bit_count) struct { unsigned char bits[(bit_count / CHAR_BIT) + 1]; }
#define get_bit(bitmap, bit) ((bitmap.bits[word_offset(bit)] & (1 << bit_offset(bit))) ? 1 : 0)
#define set_bit(bitmap, bit)   do { bitmap.bits[word_offset(bit)] |= (1 << bit_offset(bit)); } while (0)
#define unset_bit(bitmap, bit) do { bitmap.bits[word_offset(bit)] |= (0 << bit_offset(bit)); } while (0)

#define Bitmap2D(width, height) struct { Bitmap(height) row[height]; }
#define get_bit_2d(bitmap, x, y) get_bit(bitmap.row[y], x)
#define set_bit_2d(bitmap, x, y) set_bit(bitmap.row[y], x)
#define unset_bit_2d(bitmap, x, y) unset_bit(bitmap.row[y], x)

#endif /*BITMAP_H*/
