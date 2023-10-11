
#include <iostream>
#include <opencv2/opencv.hpp>
#include <opencv2/wechat_qrcode/wechat_qrcode.hpp>
#include <vector>
#include <ctime>
struct QRCodeResult {
    std::string text;
    std::vector<cv::Point2f> corners;
};

QRCodeResult decodeQRCode(const cv::Mat& image, cv::wechat_qrcode_WeChatQRCode& qrcode_detector) {
    QRCodeResult result;
    std::vector<std::vector<std::string>> decoded_info;

    result.text = " ";
    result.corners = {};

    qrcode_detector.detectAndDecode(image, decoded_info);

    if (!decoded_info.empty()&& !decoded_info[0].empty()) {
        result.text =decoded_info[0][0];
        result.corners =qrcode_detector.getCorner();
    }

    return result;
}

void drawTags(cv::Mat& image, const QRCodeResult& qrcode_result, double elapsed_time) {
    if (!qrcode_result.text.empty()){
        std::string text =qrcode_result.text;
        std::vector<cv::Point2f> corners = qrcode_result.corners;

        cv::Point2i corner_01(static_cast<int>(corners[0].x), static_cast<int>(corners[0].y));
        cv::Point2i corner_02(static_cast<int>(corners[1].x), static_cast<int>(corners[1].y));
        cv::Point2i corner_03(static_cast<int>(corners[2].x), static_cast<int>(corners[2].y));
        cv::Point2i corner_04(static_cast<int>(corners[3].x), static_cast<int>(corners[3].y));

        // Draw each edge
        cv::line(image, corner_01, corner_02, cv::Scalar(255, 0, 0), 2);
        cv::line(image, corner_02, corner_03, cv::Scalar(255, 0, 0), 2);
        cv::line(image, corner_03, corner_04, cv::Scalar(0, 255, 0), 2);
        cv::line(image, corner_04, corner_01, cv::Scalar(0, 255, 0), 2);

        // Draw text
        cv::putText(image, text, cv::Point(10, 55), cv::FONT_HERSHEY_SIMPLEX, 0.75, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
    }

    
    cv::putText(image, "Elapsed Time: " + std::to_string(elapsed_time * 1000) + "ms", cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2, cv::LINE_AA);
}

int main() {
    int cap_device = 0;
    int cap_width = 960;
    int cap_height = 540;

    cv::VideoCapture cap(cap_device);
    cap.set(cv::CAP_PROP_FRAME_WIDTH, cap_width);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, cap_height);

    cv::wechat_qrcode_WeChatQRCode qrcode_detector("model/detect.prototxt", "model/detect.caffemodel", "model/sr.prototxt", "model/sr.caffemodel");

    double elapsed_time = 0;

    while (true) {
        double start_time = static_cast<double>(cv::getTickCount());

        cv::Mat image;
        cap >> image;

        if (image.empty()) {
            break;
        }

        cv::Mat debug_image = image.clone();

        QRCodeResult qrcode_result = decodeQRCode(image, qrcode_detector);

        drawTags(debug_image, qrcode_result, elapsed_time);

        elapsed_time = (static_cast<double>(cv::getTickCount()) - start_time) / cv::getTickFrequency();

        cv::imshow("QR Code Detector Demo", debug_image);

        int key = cv::waitKey(1);
        if (key == 27) {  // ESC
            break;
        }
    }

    return 0;
}

        static void DrawTags(Mat image, List<string> qrCodeTexts, List<RotatedRect> qrCodeCorners, double elapsedMillis)
        {
            for (int i = 0; i < qrCodeTexts.Count; i++)
            {
                string text = qrCodeTexts[i];
                RotatedRect corner = qrCodeCorners[i];

                PointF[] boxPoints = CvInvoke.BoxPoints(corner);

                Point[] points = Array.ConvertAll(boxPoints, Point.Round);

      
                CvInvoke.Line(image, points[0], points[1], new MCvScalar(255, 0, 0), 2);
                CvInvoke.Line(image, points[1], points[2], new MCvScalar(255, 0, 0), 2);
                CvInvoke.Line(image, points[2], points[3], new MCvScalar(0, 255, 0), 2);
                CvInvoke.Line(image, points[3], points[0], new MCvScalar(0, 255, 0), 2);


                CvInvoke.PutText(image, text, points[0], FontFace.HersheySimplex, 0.5, new MCvScalar(0, 255, 0), 1, LineType.AntiAlias);
            }

          
            CvInvoke.PutText(image, "Elapsed Time: " + elapsedMillis.ToString("F1") + "ms", new Point(10, 30), FontFace.HersheySimplex, 0.8, new MCvScalar(0, 255, 0), 2, LineType.AntiAlias);
        }
    }
}
