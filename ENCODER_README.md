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

### 1. ISR Terpisah untuk Setiap Pin
Setiap pin encoder sekarang memiliki ISR sendiri:
- `onEncoder1A()` untuk encoder1_a
- `onEncoder1B()` untuk encoder1_b
- `onEncoder2A()` untuk encoder2_a
- `onEncoder2B()` untuk encoder2_b

### 2. Dua Counter Terpisah
- `pulsa1` untuk encoder 1
- `pulsa2` untuk encoder 2

### 3. Inisialisasi Semaphore
```cpp
timerSemaphore = xSemaphoreCreateBinary();
```

### 4. Proteksi Critical Section di Loop
```cpp
portENTER_CRITICAL(&timerMux);
long pulsa1_copy = pulsa1;
long pulsa2_copy = pulsa2;
portEXIT_CRITICAL(&timerMux);
```

### 5. Delay di Loop
```cpp
delay(100);  // Menghindari serial flooding
```

## Cara Kerja Encoder Quadrature

Encoder quadrature memiliki 2 output (A dan B) yang bergeser fase 90 derajat:
- Jika A naik saat B rendah → rotasi searah jarum jam (increment)
- Jika A naik saat B tinggi → rotasi berlawanan jarum jam (decrement)

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
- Pin 36, 34, 39, 35 adalah pin input-only pada beberapa board ESP32
- Pastikan encoder memiliki pull-up resistor jika diperlukan
- Untuk pembacaan yang lebih akurat, pertimbangkan untuk menambahkan debouncing
