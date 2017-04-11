# EDB File Format {#md_edb_format}

EDB stands for "EAARL database". These files are also refered to as EAARL index
files. The EDB file allows the user to quickly look up any raster record in a
collection of TLD files. The EDB file also defines the raster numbers for a
flight: a raster's raster number is its one-based index into the EDB file's
records section.

## File Format

The EDB file format has three sections:

1. Header, specifying the layout and contents of the file

2. Records, providing information about the rasters in the associated TLD files.

3. Filenames, providing the filenames for the associated TLD files
   (referenced by index from the records).

The file stores values in little-endian format.

The file starts with the header. The records then immediately follow, starting
at byte offset 12. The filenames are located at the *files_offset* specified in
the header.

### Header

The header specifies the layout and contents of the file.

The EDB's file header is 12 bytes long.

| Item         | Format   | Size    |
| ------------ | -------- | ------- |
| files_offset | uint32_t | 4 bytes |
| record_count | uint32_t | 4 bytes |
| file_count   | uint32_t | 4 bytes |

- **files_offset:** The offset into the file where the filenames portion of the
  file is stored. (This is typically equal to 12 + 20 * record_count, since the
  filenames usually start immediately after the records. However, this is not
  strictly enforced.)

- **record_count:** Number of records in the record section of the file.

- **file_count:** Number of filenames in the filenames section of the file.

### Record

Each record provides information about a single raster.

Each record is 20 bytes long.

| Item          | Format   | Size    |
| ------------- | -------- | ------- |
| time_seconds  | uint32_t | 4 bytes |
| time_fraction | uint32_t | 4 bytes |
| record_offset | uint32_t | 4 bytes |
| record_length | uint32_t | 4 bytes |
| file_index    | int16_t  | 2 bytes |
| pulse_count   | uint8_t  | 1 byte  |
| digitizer     | uint8_t  | 1 byte  |

- **time_seconds:** The integer seconds portion of the timestamp for the
  raster. This needs to be combined with *time_fraction* to derive the full
  timestamp. This field corresponds to the *time_fraction* field of the raster
  header in the TLD file. However, the value is sometimes adjusted in the EDB
  file to incorporate an offset when the EAARL system time is out of sync with
  real time.

- **time_fraction:** The fractional seconds portion of the timestamp for the
  raster. This needs to be combined with *time_seconds* to derive the full
  timestamp. The value is stored in a hardware-specific format; to convert to
  seconds, multiply by 1.6e-6.

- **record_offset:** Offset in bytes to the start of the record in the TLD
  file.

- **record_length:** Length in bytes of the record in the TLD file. This
  corresponds to the *record_length* field of the record header in the TLD
  file.

- **file_index:** Index into the filenames section of the EDB file for the TLD
  file that has this record. The index is one-based.

- **pulse_count:** Number of pulses in the raster. This corresponds to the
  *pulse_count* field of the raster header in the TLD file.

- **digitizer:** Specifies which digitizer was used for the waveforms, either 0
  or 1. This corresponds to the *digitizer* field of the raster header in the
  TLD file.

### Filename

Each filename has a variable length. Filenames are stored as length-prefixed
strings. The length prefix is 2 bytes and is followed by the string without a
null terminator.

| Item     | Format   | Size              |
| -------- | -------- | ----------------- |
| length   | uint16_t | 2 bytes           |
| filename | char[]   | varies per length |
