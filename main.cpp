#include <opencv2/opencv.hpp>
#include <ZXing/ReadBarcode.h>
#include <ZXing/BarcodeFormat.h>
#include <ZXing/ReaderOptions.h>
#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

// Median blur
cv::Mat apply_median(const cv::Mat& img, int ksize = 7) {
    cv::Mat out;
    cv::medianBlur(img, out, ksize);
    return out;
}
cv::Mat to_gray(const cv::Mat& img) {
    cv::Mat gray;
    if (img.channels() == 3)
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    else
        gray = img.clone();
    return gray;
}

// Gaussian blur
cv::Mat apply_gaussian(const cv::Mat& img, int ksize = 7, double sigma = 1.5) {
    cv::Mat out;
    cv::GaussianBlur(img, out, cv::Size(ksize, ksize), sigma);
    return out;
}

// Histogram eşitleme (sadece gri görüntüde anlamlıdır)
cv::Mat apply_hist_eq(const cv::Mat& img) {
    cv::Mat gray, out;
    if (img.channels() == 3)
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    else
        gray = img.clone();
    cv::equalizeHist(gray, out);
    return out;
}

// Adaptive Threshold
cv::Mat apply_adaptive_threshold(const cv::Mat& img, int blockSize = 33, int C = 11) {
    cv::Mat gray, out;
    if (img.channels() == 3)
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    else
        gray = img.clone();
    cv::adaptiveThreshold(gray, out, 255, cv::ADAPTIVE_THRESH_MEAN_C,
                          cv::THRESH_BINARY, blockSize, C);
    return out;
}

// Kontrast artır
cv::Mat increase_contrast(const cv::Mat& img, double alpha = 1.5, int beta = 0) {
    cv::Mat out;
    img.convertTo(out, -1, alpha, beta);
    return out;
}

// Döndürme
cv::Mat rotate_image(const cv::Mat& src, double angle) {
    cv::Point2f center(src.cols/2.0F, src.rows/2.0F);
    cv::Mat rot = cv::getRotationMatrix2D(center, angle, 1.0);
    cv::Mat dst;
    cv::warpAffine(src, dst, rot, src.size(), cv::INTER_LINEAR, cv::BORDER_REPLICATE);
    return dst;
}

// Barkod okuma
std::string try_decode(const cv::Mat& img, const ZXing::ReaderOptions& options) {
    ZXing::ImageView zxingImage(
        img.data, img.cols, img.rows,
        img.channels() == 1 ? ZXing::ImageFormat::Lum : ZXing::ImageFormat::BGR
    );
    auto result = ZXing::ReadBarcode(zxingImage, options);
    return result.isValid() ? result.text() : "";
}

// Keskinleştirme
cv::Mat apply_sharpen(const cv::Mat& img) {
    cv::Mat out;
    cv::Mat kernel = (cv::Mat_<float>(3,3) <<
         0, -1,  0,
        -1,  5, -1,
         0, -1,  0);
    cv::filter2D(img, out, img.depth(), kernel);
    return out;
}

// genişletme
cv::Mat apply_dilate(const cv::Mat& img, int ksize = 3) {
    cv::Mat out;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::dilate(img, out, kernel);
    return out;
}

// daraltma
cv::Mat apply_erode(const cv::Mat& img, int ksize = 3) {
    cv::Mat out;
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(ksize, ksize));
    cv::erode(img, out, kernel);
    return out;
}

// Basit threshold
cv::Mat apply_simple_threshold(const cv::Mat& img, double thresh = 127) {
    cv::Mat gray, out;
    if (img.channels() == 3)
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    else
        gray = img.clone();
    cv::threshold(gray, out, thresh, 255, cv::THRESH_BINARY);
    return out;
}

// CLAHE
cv::Mat apply_clahe(const cv::Mat& img) {
    cv::Mat gray, out;
    if (img.channels() == 3)
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);
    else
        gray = img.clone();
    cv::Ptr<cv::CLAHE> clahe = cv::createCLAHE(2.0, cv::Size(8,8));
    clahe->apply(gray, out);
    return out;
}

// Resize
cv::Mat apply_resize(const cv::Mat& img, double scale = 2.0) {
    cv::Mat out;
    cv::resize(img, out, cv::Size(), scale, scale, cv::INTER_CUBIC);
    return out;
}


int main() {
    std::string folder_path = "/Users/ferhan/Desktop/son_data";
    std::string single_image_path = "/Users/ferhan/Desktop/son_data/nok1_60.jpg"; // Tekli mod için yol (örnek: "/Users/ferhan/Desktop/son_data/ok1_1.jpg")
    // single_image_path = "/Users/ferhan/Desktop/son_data/ok1_1.jpg"; // Denemek için bunu açabilirsin


    // İstediğin işlemleri true/false ile seç!
    bool use_median = true;
    bool use_gaussian = false;
    bool use_hist_eq = false;
    bool use_adapt_thresh = false;
    bool use_contrast = false;
    bool use_rotate = true;
    bool use_sharpen = false;
    bool use_dilate = false;
    bool use_erode = false;
    bool use_simple_thresh = false;
    bool use_clahe = false;
    bool use_resize = true;
    bool use_gray = false;

        ZXing::ReaderOptions options;
    options.setTryHarder(true);
    options.setFormats({ZXing::BarcodeFormat::DataMatrix});

if (!single_image_path.empty()) {
    cv::Mat img = cv::imread(single_image_path, cv::IMREAD_COLOR);
    if (img.empty()) {
        std::cout << single_image_path << ": Tekil resim okunamadı!" << std::endl;
        return 1;
    }

    cv::Mat current_img = img;
    std::string tag = "[ORIJINAL]";

    if (use_resize) { current_img = apply_resize(current_img, 2.0); tag += "[RESIZE]"; }
    if (use_gray) { current_img = to_gray(current_img); tag += "[GRAY]"; }
    if (use_median) { current_img = apply_median(current_img); tag += "[MEDIAN]"; }
    if (use_gaussian) { current_img = apply_gaussian(current_img); tag += "[GAUSSIAN]"; }
    if (use_hist_eq) { current_img = apply_hist_eq(current_img); tag += "[HIST-EQ]"; }
    if (use_simple_thresh) { current_img = apply_simple_threshold(current_img, 127); tag += "[SIMPLE-THRESH]"; }
    if (use_clahe) { current_img = apply_clahe(current_img); tag += "[CLAHE]"; }
    if (use_sharpen) { current_img = apply_sharpen(current_img); tag += "[SHARPEN]"; }
    if (use_dilate) { current_img = apply_dilate(current_img, 3); tag += "[DILATE]"; }
    if (use_erode) { current_img = apply_erode(current_img, 3); tag += "[ERODE]"; }
    if (use_adapt_thresh) { current_img = apply_adaptive_threshold(current_img); tag += "[ADAPT-THRESH]"; }
    if (use_contrast) { current_img = increase_contrast(current_img, 2.0, 0); tag += "[KONTRAST]"; }

    if (use_rotate) {
        bool bulundu = false;
        for (int angle = 10; angle < 360; angle += 10) {
            cv::Mat rotated = rotate_image(current_img, angle);
            std::string barkod = try_decode(rotated, options);
            if (!barkod.empty()) {
                std::cout << tag << "[DONDURME " << angle << "]: " << barkod << std::endl;
                // Sonucu göster
                cv::imshow("Orijinal", img);
                cv::imshow("Islenmis (Rotate)", rotated);
                cv::waitKey(0);
                bulundu = true;
                break;
            }
        }
        if (!bulundu) {
            std::cout << tag << "[DONDURME]: Barkod bulunamadı!" << std::endl;
            cv::imshow("Orijinal", img);
            cv::imshow("Islenmis (Rotate)", current_img);
            cv::waitKey(0);
        }
    } else {
        std::string barkod = try_decode(current_img, options);
        if (!barkod.empty())
            std::cout << tag << ": " << barkod << std::endl;
        else
            std::cout << tag << ": Barkod bulunamadı!" << std::endl;

        cv::imshow("Orijinal", img);
        cv::imshow("Islenmis", current_img);
        cv::waitKey(0);
    }
    return 0;
}


    for (const auto& entry : fs::directory_iterator(folder_path)) {
        std::string filename = entry.path().string();
        if (!filename.ends_with(".jpg")) continue;

        cv::Mat img = cv::imread(filename, cv::IMREAD_COLOR);
        if (img.empty()) {
            std::cout << filename << ": Resim okunamadı!" << std::endl;
            continue;
        }

        std::cout << "==== " << filename << " ====" << std::endl;
        cv::Mat current_img = img;
        std::string tag = "[ORIJINAL]";
        if (use_gray) {
            current_img = to_gray(current_img);
            tag += "[GRAY]";
        }

        if (use_resize) {
            current_img = apply_resize(current_img, 2.0); // istersen scale'ı değiştir
            tag += "[RESIZE]";
        }
        if (use_median) {
            current_img = apply_median(current_img);
            tag += "[MEDIAN]";
        }
        if (use_gaussian) {
            current_img = apply_gaussian(current_img);
            tag += "[GAUSSIAN]";
        }
        if (use_hist_eq) {
            current_img = apply_hist_eq(current_img);
            tag += "[HIST-EQ]";
        }
        if (use_simple_thresh) {
            current_img = apply_simple_threshold(current_img, 127);
            tag += "[SIMPLE-THRESH]";
        }
        if (use_clahe) {
            current_img = apply_clahe(current_img);
            tag += "[CLAHE]";
        }
        if (use_sharpen) {
            current_img = apply_sharpen(current_img);
            tag += "[SHARPEN]";
        }
        if (use_dilate) {
            current_img = apply_dilate(current_img, 3);
            tag += "[DILATE]";
        }
        if (use_erode) {
            current_img = apply_erode(current_img, 3);
            tag += "[ERODE]";
        }

        if (use_adapt_thresh) {
            current_img = apply_adaptive_threshold(current_img);
            tag += "[ADAPT-THRESH]";
        }
        if (use_contrast) {
            current_img = increase_contrast(current_img, 2.0, 0);
            tag += "[KONTRAST]";
        }

        if (use_rotate) {
            bool bulundu = false;
            for (int angle = 10; angle < 360; angle += 10) {
                cv::Mat rotated = rotate_image(current_img, angle);
                std::string barkod = try_decode(rotated, options);
                if (!barkod.empty()) {
                    std::cout << tag << "[DONDURME " << angle << "]: " << barkod << std::endl;
                    bulundu = true;
                    break;
                }
            }
            if (!bulundu)
                std::cout << tag << "[DONDURME]: Barkod bulunamadı!" << std::endl;
        } else {
            std::string barkod = try_decode(current_img, options);
            if (!barkod.empty())
                std::cout << tag << ": " << barkod << std::endl;
            else
                std::cout << tag << ": Barkod bulunamadı!" << std::endl;
        }
    }
    return 0;
}
