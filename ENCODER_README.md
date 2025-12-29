# Encoder Interrupt Implementation

## Pertanyaan: "Apakah ini bisa?"

Ya, konsepnya **bisa**, tetapi kode asli memiliki beberapa masalah yang perlu diperbaiki.

## Masalah dalam Kode Asli

1. **ISR hanya menangani encoder1**: Meskipun semua 4 pin (encoder1 dan encoder2) dipasang interrupt, fungsi ISR `onTimer()` hanya membaca dan memproses encoder1.

2. **Tidak ada inisialisasi semaphore**: Variable `timerSemaphore` dideklarasikan tetapi tidak pernah dibuat dengan `xSemaphoreCreateBinary()`.

3. **Semaphore tidak digunakan**: Semaphore diberikan di ISR tetapi tidak pernah diambil di loop utama.

4. **Flooding serial output**: Loop utama tidak memiliki delay, sehingga akan membanjiri output serial.

5. **Race condition**: Variable `pulsa` dibaca di loop utama tanpa proteksi critical section.

6. **Logika ISR**: Satu ISR untuk semua interrupt dapat menyebabkan pembacaan yang salah karena kondisi yang berbeda untuk setiap pin.

## Perbaikan yang Dilakukan

### 1. ISR yang Disederhanakan
Setiap encoder sekarang memiliki satu ISR yang menangani perubahan pada kedua pin A dan B:
- `onEncoder1()` untuk encoder 1 (menangani pin A dan B)
- `onEncoder2()` untuk encoder 2 (menangani pin A dan B)

Logika ISR menggunakan perbandingan sederhana: jika A == B maka decrement, jika tidak maka increment.

### 2. Interrupt pada CHANGE Edge
Menggunakan `CHANGE` daripada `RISING` untuk mendapatkan resolusi 4x dan deteksi arah yang lebih baik:
```cpp
attachInterrupt(digitalPinToInterrupt(encoder1_a), onEncoder1, CHANGE);
```

### 3. Dua Counter Terpisah
- `pulsa1` untuk encoder 1
- `pulsa2` untuk encoder 2

### 4. Menghapus Semaphore yang Tidak Digunakan
Semaphore telah dihapus karena tidak diperlukan untuk aplikasi ini. Critical section sudah cukup untuk proteksi data.

### 5. Proteksi Critical Section di Loop
```cpp
portENTER_CRITICAL(&timerMux);
long pulsa1_copy = pulsa1;
long pulsa2_copy = pulsa2;
portEXIT_CRITICAL(&timerMux);
```

### 6. Delay di Loop
```cpp
delay(100);  // Menghindari serial flooding
```

## Cara Kerja Encoder Quadrature

Encoder quadrature memiliki 2 output (A dan B) yang bergeser fase 90 derajat:
- Jika A == B setelah perubahan → rotasi berlawanan jarum jam (decrement)
- Jika A != B setelah perubahan → rotasi searah jarum jam (increment)

Dengan menggunakan interrupt pada `CHANGE` (bukan `RISING`), kita mendapatkan:
- Resolusi 4x (setiap transisi pada A atau B memicu interrupt)
- Deteksi arah yang lebih akurat

## Pin Configuration

- **Encoder 1**: Pin 36 (A) dan 34 (B)
- **Encoder 2**: Pin 39 (A) dan 35 (B)

## Testing

Untuk menguji kode ini:
1. Upload ke ESP32
2. Putar encoder 1 dan 2
3. Monitor serial output untuk melihat perubahan PULSA1 dan PULSA2

## Catatan Penting

- Pin GPIO yang digunakan harus mendukung interrupt pada ESP32
- Pin 36 dan 39 adalah pin input-only pada ESP32 dan tidak memiliki internal pull-up
- **Penting**: Pastikan encoder memiliki pull-up resistor eksternal (biasanya 10kΩ ke 3.3V)
- Interrupt pada CHANGE memberikan resolusi 4x lebih tinggi daripada RISING
- Untuk mengurangi noise, pertimbangkan untuk menambahkan capacitor (100nF) pada setiap pin encoder
