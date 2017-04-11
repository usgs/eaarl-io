# TLD File Format {#md_tld_format}

TLD stands for "Type, Length, Data", which describes the general storage
paradigm used. Each record starts with a short header than specifies the record
length and the record type. Following that is the data, which is interpreted
based on the record's type. This library is only concerned with type 5, which
is used for EAARL-A and EAARL-B raster data.

## File Format

A TLD file is a series of variable length records. Each record has two parts:

1. Header, specifying the type and size of the record.
2. Data, which is to be interpreted based on the record type.

The eaarl-io library can decode the header for any record type, but will only
decode the data for type = 5 (EAARL-A and EAARL-B raster data).

The data section of a raster record has the following structure:

1. Raster header

2. Series of pulse records, each of which consist of:

   1. Pulse header

   2. Waveform data

The file stores values in little-endian format.

### Record Header

| Item          | Format   | Size    |
| ------------- | -------- | ------- |
| record_length | uint24_t | 3 bytes |
| record_type   | uint8_t  | 1 byte  |

- **record_length:** The length of the record, including this header.
- **record_type:** The type identifier for the record. For EAARL-A and EAARL-B
  raster data, this is always 5.

Note that *record_length* is more authorative than any length information in
the record. If the record suggests that it should be longer than the
*record_length* allows, then the record must be truncated.

### Raster Header

| Item                               | Format   | Size    |
| ---------------------------------- | -------- | ------- |
| time_seconds                       | uint32_t | 4 bytes |
| time_fraction                      | uint32_t | 4 bytes |
| sequence_number                    | uint32_t | 4 bytes |
| pulse_count and digitizer bitfield | uint16_t | 2 bytes |

The *pulse_count* and *digitizer* fields are stored together in a 2-byte
unsigned integer. The *pulse_count* is the lower 15 bits of the integer. The
*digitizer* is the high bit, 1 or 0. Because the integer is stored in
little-endian format, the bit for *digitizer* is located as the high bit of the
second byte when viewing the raw bytes of the file.

- **time_seconds:** The integer seconds portion of the timestamp for the
  raster. This needs to be combined with *time_fraction* to derive the full
  timestamp.

- **time_fraction:** The fractional seconds portion of the timestamp for the
  raster. This needs to be combined with *time_seconds* to derive the full
  timestamp. The value is stored in a hardware-specific format; to convert to
  seconds, multiply by 1.6e-6.

- **sequence_number:** This is a simple sequence number. The value cycles and
  repeats and is thus not suitable for uniquely identifying rasters. It is not
  used for anything and should be ignored.

- **pulse_count:** Number of pulses in the raster. This specifies how many
  pulse records will follow.

- **digitizer:** The digitizer used for the waveforms, either 0 or 1.

### Pulse Header

| Item                                     | Format     | Size    |
| ---------------------------------------- | ---------- | ------- |
| time_offset                              | uint24_t   | 3 bytes |
| rx_count                                 | uint8_t    | 1 byte  |
| bias_tx                                  | uint8_t    | 1 byte  |
| bias_rx                                  | uint8_t[4] | 4 bytes |
| scan_angle_counts                        | int16_t    | 2 bytes |
| range, thresh_tx, and thresh_rx bitfield | uint16_t   | 2 bytes |

The *range*, *thresh_tx*, and *thresh_rx* fields are stored together in a
2-byte unsigned integer. The *range* is the lower 14 bits of the integer. The
*thresh_tx* is bit 15 and *thresh_rx* is bit 16 (each are 1 or 0). Because the
integer is stored in little-endian format, the bits for *thresh_tx* and
*thresh_rx* are the high bits of the second byte when viewing the raw bytes of
the file.

- **time_offset:** Additional fractional time elapsed since start of raster.
  The value is stored in a hardware-specific format; to convert to seconds,
  multiply by 1.6e-6. This needs to be added to the raster's timestamp
  (*time_seconds* and *time_fraction*) to derive the timestamp for the pulse.

- **rx_count:** Number of waveforms in the pulse record. This should never
  exceed 4.

- **bias_tx:** Transmit range bias in nanoseconds.

- **bias_rx:** Channel range biases in nanoseconds. There are four values
  regardless of *rx_count*. If *rx_count* < 4, then the extra values are
  meaningless.

- **scan_angle_counts:** Angle of the scanner. The value is stored in a
  hardware-specific format; to convert to degrees, multiply by 0.045.

- **range:** Range measurement of the laser pulse, in nanoseconds. This measure
  the travel time from the mirror to the start of the raster and back.

- **thresh_tx:** Transmit waveform hardware threshold failure. 1 indicates that
  the transmit waveform failed the hardware-defined threshold.

- **thresh_rx:** Return waveform hardware threshold failure. 1 indicates that
  the return waveforms failed the hardware-defined threshold.

### Waveform Data

| Item        | Format   | Size                   |
| ----------- | -------- | ---------------------- |
| data_length | uint16_t | 2 bytes                |
| data        |          | varies per data_length |

The data consists of the transmit waveform followed by up to four return
waveforms. The waveform count is as specified by the *rx_count* field of
the pulse header. Each waveform is stored as a length value followed by the
waveform data.

The transmit waveform is stored as:

| Item   | Format          | Size              |
| ------ | --------------- | ----------------- |
| tx_len | uint8_t         | 1 byte            |
| tx     | unsigned char[] | varies per tx_len |

Each return waveform is stored as:

| Item   | Format          | Size              |
| ------ | --------------- | ----------------- |
| rx_len | uint16_t        | 2 bytes           |
| rx     | unsigned char[] | varies per rx_len |

Note that *data_length* is more authoriative than the *tx_len* and *rx_len*
values. If a waveform's length would extend beyond the space designated by
*data_length*, then it should be truncated.

### Example record

Follows is an example of how the components all fit together. The example
raster has two pulses. The first pulse has one waveform and the second pulse has
four waveforms.

| Section       | Subsection    | Field             |
| ------------- | ------------- | ----------------- |
| Record Header |               | record_length     |
| Record Header |               | record_type       |
| Raster Header |               | time_seconds      |
| Raster Header |               | time_fraction     |
| Raster Header |               | sequence_number   |
| Raster Header |               | bitfield          |
| Pulse 0       | Pulse Header  | time_offset       |
| Pulse 0       | Pulse Header  | rx_count          |
| Pulse 0       | Pulse Header  | bias_tx           |
| Pulse 0       | Pulse Header  | bias_rx[4]        |
| Pulse 0       | Pulse Header  | scan_angle_counts |
| Pulse 0       | Pulse Header  | bitfield          |
| Pulse 0       | Waveform Data | data_length       |
| Pulse 0       | Waveform Data | tx_len            |
| Pulse 0       | Waveform Data | tx[]              |
| Pulse 0       | Waveform Data | rx_len[0]         |
| Pulse 0       | Waveform Data | rx[0][]           |
| Pulse 1       | Pulse Header  | time_offset       |
| Pulse 1       | Pulse Header  | rx_count          |
| Pulse 1       | Pulse Header  | bias_tx           |
| Pulse 1       | Pulse Header  | bias_rx[4]        |
| Pulse 1       | Pulse Header  | scan_angle_counts |
| Pulse 1       | Pulse Header  | bitfield          |
| Pulse 1       | Waveform Data | data_length       |
| Pulse 1       | Waveform Data | tx_len            |
| Pulse 1       | Waveform Data | tx[]              |
| Pulse 1       | Waveform Data | rx_len[0]         |
| Pulse 1       | Waveform Data | rx[0][]           |
| Pulse 1       | Waveform Data | rx_len[1]         |
| Pulse 1       | Waveform Data | rx[1][]           |
| Pulse 1       | Waveform Data | rx_len[2]         |
| Pulse 1       | Waveform Data | rx[2][]           |
| Pulse 1       | Waveform Data | rx_len[3]         |
| Pulse 1       | Waveform Data | rx[3][]           |

