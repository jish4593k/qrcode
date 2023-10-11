import cv2
import numpy as np

def detect_and_decode(qr_decoder, image):
    qr_code_texts = []
    qr_code_corners = []
    
    gray_image = cv2.cvtColor(image, cv2.COLOR_BGR2GRAY)
    gray_image = cv2.GaussianBlur(gray_image, (5, 5), 0)
    
    canny_image = cv2.Canny(gray_image, 100, 200)
    
    contours, _ = cv2.findContours(canny_image, cv2.RETR_LIST, cv2.CHAIN_APPROX_SIMPLE)
    
    for contour in contours:
        if len(contour) == 4:
            epsilon = 0.02 * cv2.arcLength(contour, True)
            approx = cv2.approxPolyDP(contour, epsilon, True)
            
            if len(approx) == 4 and cv2.isContourConvex(approx):
                box = cv2.minAreaRect(approx)
                box_points = cv2.boxPoints(box).astype(int)
                box_points = sorted(box_points, key=lambda p: (p[0], p[1]))
                
                qr_code_corners.append(box)
                
                _, decoded_info, _ = qr_decoder.detectAndDecode(image)
                qr_code_texts.append(decoded_info)
    
    return qr_code_texts, qr_code_corners

def draw_tags(image, qr_code_texts, qr_code_corners, elapsed_millis):
    for text, corner in zip(qr_code_texts, qr_code_corners):
        box_points = np.int0(cv2.boxPoints(corner))
        cv2.drawContours(image, [box_points], 0, (0, 255, 0), 2)
        cv2.putText(image, text, tuple(box_points[0]), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 1, cv2.LINE_AA)
    
    cv2.putText(image, f"Elapsed Time: {elapsed_millis:.1f}ms", (10, 30), cv2.FONT_HERSHEY_SIMPLEX, 0.8, (0, 255, 0), 2, cv2.LINE_AA)

def main():
    cap_device = 0
    cap_width = 960
    cap_height = 540
    
    cap = cv2.VideoCapture(cap_device)
    cap.set(cv2.CAP_PROP_FRAME_WIDTH, cap_width)
    cap.set(cv2.CAP_PROP_FRAME_HEIGHT, cap_height)
    
    qr_decoder = cv2.QRCodeDetector_create()
    
    elapsed_millis = 0

    while True:
        start_time = cv2.getTickCount()
        
        ret, frame = cap.read()
        
        if not ret:
            break
        
        debug_image = frame.copy()
        
        qr_code_texts, qr_code_corners = detect_and_decode(qr_decoder, debug_image)
        
        draw_tags(debug_image, qr_code_texts, qr_code_corners, elapsed_millis)
        
        elapsed_millis = (cv2.getTickCount() - start_time) / cv2.getTickFrequency() * 1000
        
        cv2.imshow("QR Code Detector Demo", debug_image)
        
        key = cv2.waitKey(1)
        if key == 27:  # ESC key
            break

    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
