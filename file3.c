#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <cv.h>
#include <highgui.h>

// Define constants
#define MAX_TEXT_LENGTH 256

// Function to get arguments
void getArgs(int argc, char** argv, int* device, int* width, int* height) {
    int i;
    *device = 0;
    *width = 960;
    *height = 540;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--device") == 0) {
            if (i + 1 < argc) {
                *device = atoi(argv[i + 1]);
                i++;
            }
        } else if (strcmp(argv[i], "--width") == 0) {
            if (i + 1 < argc) {
                *width = atoi(argv[i + 1]);
                i++;
            }
        } else if (strcmp(argv[i], "--height") == 0) {
            if (i + 1 < argc) {
                *height = atoi(argv[i + 1]);
                i++;
            }
        }
    }
}

// Function to draw tags
void drawTags(IplImage* image, const char* text, CvPoint2D32f* corners, double elapsedMillis) {
    CvPoint cornersInt[4];
    int i;

    for (i = 0; i < 4; i++) {
        cornersInt[i].x = cvRound(corners[i].x);
        cornersInt[i].y = cvRound(corners[i].y);
    }

    // Draw each edge
    cvLine(image, cornersInt[0], cornersInt[1], CV_RGB(255, 0, 0), 2, CV_AA, 0);
    cvLine(image, cornersInt[1], cornersInt[2], CV_RGB(255, 0, 0), 2, CV_AA, 0);
    cvLine(image, cornersInt[2], cornersInt[3], CV_RGB(0, 255, 0), 2, CV_AA, 0);
    cvLine(image, cornersInt[3], cornersInt[0], CV_RGB(0, 255, 0), 2, CV_AA, 0);

    // Draw text
    CvFont font;
    cvInitFont(&font, CV_FONT_HERSHEY_SIMPLEX, 0.5, 0.5, 0, 1, CV_AA);
    cvPutText(image, text, cornersInt[0], &font, CV_RGB(0, 255, 0));
    
    // Draw elapsed time
    char timeText[MAX_TEXT_LENGTH];
    snprintf(timeText, sizeof(timeText), "Elapsed Time: %.1fms", elapsedMillis);
    cvPutText(image, timeText, cvPoint(10, 30), &font, CV_RGB(0, 255, 0));
}

int main(int argc, char** argv) {
    int capDevice, capWidth, capHeight;
    getArgs(argc, argv, &capDevice, &capWidth, &capHeight);

    CvCapture* cap = cvCaptureFromCAM(capDevice);
    if (!cap) {
        fprintf(stderr, "Error: Could not initialize video capture!\n");
        return -1;
    }

    cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_WIDTH, capWidth);
    cvSetCaptureProperty(cap, CV_CAP_PROP_FRAME_HEIGHT, capHeight);

    CvQRCodeDetector qrCodeDetector = cvCreateQRCodeDetector();

    double elapsedMillis = 0;

    while (1) {
        double startTime = (double)clock() / CLOCKS_PER_SEC;

        IplImage* frame = cvQueryFrame(cap);
        if (!frame) {
            break;
        }

        IplImage* debugImage = cvCloneImage(frame);

        // Detect and decode
        const char* result = cvDetectAndDecodeQRCode(&qrCodeDetector, frame, NULL, NULL);

        // Draw tags
        if (result) {
            CvPoint2D32f corners[4];
            cvDetectQRCode(frame, NULL, &corners[0], NULL);
            drawTags(debugImage, result, corners, elapsedMillis);
        }

        elapsedMillis = (((double)clock() / CLOCKS_PER_SEC) - startTime) * 1000;

        cvShowImage("QR Code Detector Demo", debugImage);

        char key = cvWaitKey(1);
        if (key == 27) {  // ESC
            break;
        }

        cvReleaseImage(&debugImage);
    }

    cvReleaseCapture(&cap);
    cvDestroyAllWindows();

    return 0;
}
