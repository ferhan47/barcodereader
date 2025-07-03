import cv2
import numpy as np
from dbr import BarcodeReader  # pip install dbr
import glob

def median_and_rotate(img, kernel_size=7, rot_angle=0):
    # Median filtre (RGB)
    med = cv2.medianBlur(img, kernel_size)
    # Rotasyon
    if rot_angle != 0:
        (h, w) = med.shape[:2]
        M = cv2.getRotationMatrix2D((w/2, h/2), rot_angle, 1.0)
        med = cv2.warpAffine(med, M, (w, h), borderMode=cv2.BORDER_REPLICATE)
    return med

def read_barcode_dynamsoft(img):
    reader = BarcodeReader()
    results = reader.decode_buffer(img)
    if results:
        return [r.barcode_text for r in results]
    return []

def process_image(path):
    img = cv2.imread(path, cv2.IMREAD_COLOR)
    if img is None:
        return path, "Resim okunamadı", None, None

    for angle in range(0, 360, 10):
        processed = median_and_rotate(img, kernel_size=7, rot_angle=angle)
        barcodes = read_barcode_dynamsoft(processed)
        if barcodes:
            return path, "OKUNDU", barcodes[0], angle if angle > 0 else None

    return path, "OKUNAMADI", None, None

img_paths = glob.glob("/Users/ferhan/Desktop/son_data/*.jpg")
results = []
for p in img_paths:
    results.append(process_image(p))



okunan = sum(1 for r in results if r[1] == "OKUNDU")
okunamayan = sum(1 for r in results if r[1] == "OKUNAMADI")
print(f"Okunan: {okunan} | Okunamayan: {okunamayan} | Başarı oranı: %{100*okunan/(okunan+okunamayan):.1f}")

# Rotasyon ile okunanlar
rotasyonla_okunan = [r for r in results if r[1] == "OKUNDU" and r[3] is not None and r[3] != 0]
if rotasyonla_okunan:
    print("\nRotasyonla okunanlar:")
    for r in rotasyonla_okunan:
        print(f"{r[0]} | Barkod: {r[2]} | Rotasyon açısı: {r[3]}°")
else:
    print("\nHiçbiri rotasyonla okunmadı.")
