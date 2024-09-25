import cv2
import numpy as np
import time
import serial

def capture_image(cap):
    """
    Capture une image depuis la caméra et retourne l'image en niveaux de gris.
    """
    ret, frame = cap.read()

    if not ret:
        print("Erreur : Échec de la capture d'image")
        cap.release()
        exit()

    # Convertir l'image en niveaux de gris
    gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    return gray_frame

def calibrate(gray_frame):
    """
    Calibrer la position de référence et calculer le ratio de pixels par centimètre.
    """
    # Définir une plage de seuils pour détecter le rectangle blanc
    _, white_mask = cv2.threshold(gray_frame, 100, 120, cv2.THRESH_BINARY)

    # Trouver les contours du rectangle blanc
    contours, _ = cv2.findContours(white_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    if len(contours) == 0:
        print("Aucun rectangle blanc détecté")
        exit()

    # Trouver le plus grand contour (en supposant que c'est le rectangle blanc)
    largest_contour = max(contours, key=cv2.contourArea)
    a, b, w, h = cv2.boundingRect(largest_contour)

    # Dimensions réelles du rectangle
    w_cm = 60
    h_cm = 30
    w_pix_cm = w / w_cm
    h_pix_cm = h / h_cm

    print(f"Dimensions du rectangle en pixel : longueur = {w} pixels, largeur= {h} pixels")
    print(f"Dimensions réelles : longueur = {w_cm} cm, largeur= {h_cm} cm")
    print(f"Ratio de pixels par cm : longueur = {w_pix_cm} pixels/cm, largeur= {h_pix_cm} pixels/cm")

    # Créer un masque pour le rectangle blanc
    mask = np.zeros_like(gray_frame)
    cv2.drawContours(mask, [largest_contour], -1, 255, thickness=cv2.FILLED)
    cv2.imwrite("calib_mask_blanc.png", mask)

    # Appliquer le masque pour ne garder que la région à l'intérieur du rectangle blanc
    roi = cv2.bitwise_and(gray_frame, gray_frame, mask=mask)

    # Définir une plage de seuils pour détecter les zones noires dans la ROI
    _, black_mask = cv2.threshold(roi, 80, 255, cv2.THRESH_BINARY_INV)
    cv2.imwrite('calib_mask_noir.png', black_mask)

    may = cv2.bitwise_and(black_mask, mask)
    cv2.imwrite('calib_supperposition.png', may)

    # Trouver les contours des zones noires dans la ROI
    black_contours, _ = cv2.findContours(may, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    output = cv2.cvtColor(may, cv2.COLOR_GRAY2BGR)

    if len(black_contours) == 0:
        print("Aucune zone noire détectée à l'intérieur du rectangle blanc")
        return None, None, None, None
    else:
        largest_black_contour = max(black_contours, key=cv2.contourArea)
        M = cv2.moments(largest_black_contour)
        if M["m00"] != 0:
            cx = int(M["m10"] / M["m00"])
            cy = int(M["m01"] / M["m00"])
            print("Position du coin: x = {} pixels, y = {} pixels".format(cx, cy))

            cv2.drawContours(output, [largest_black_contour], -1, (0, 255, 0), 2)
            cv2.circle(output, (cx, cy), 5, (255, 0, 0), -1)
            cv2.imwrite('calib_image_finale.png', output)
            return cx, cy, w_pix_cm, h_pix_cm
        else:
            print("Erreur : Calcul du centre du contour noir échoué")
            return None, None, None, None

def detect_object(gray_frame, coin_x, coin_y, rapport_x, rapport_y):
    """
    Détecter la position de l'objet par rapport au coin de calibration.
    """
    _, white_mask = cv2.threshold(gray_frame, 100, 120, cv2.THRESH_BINARY)
    contours, _ = cv2.findContours(white_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

    if len(contours) == 0:
        print("Aucun rectangle blanc détecté")
        exit()

    largest_contour = max(contours, key=cv2.contourArea)
    mask = np.zeros_like(gray_frame)
    cv2.drawContours(mask, [largest_contour], -1, 255, thickness=cv2.FILLED)
    cv2.imwrite("Masque Rectangle Blanc.png", mask)

    roi = cv2.bitwise_and(gray_frame, gray_frame, mask=mask)
    cv2.imwrite('roi.png', roi)

    _, black_mask = cv2.threshold(roi, 50, 255, cv2.THRESH_BINARY_INV)
    cv2.imwrite('black_mask_within_white_rectangle.png', black_mask)

    may = cv2.bitwise_and(black_mask, mask)
    cv2.imwrite('may.png', may)

    black_contours, _ = cv2.findContours(may, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    output = cv2.cvtColor(may, cv2.COLOR_GRAY2BGR)

    if len(black_contours) == 0:
        print("Aucune zone noire détectée à l'intérieur du rectangle blanc")
    else:
        largest_black_contour = max(black_contours, key=cv2.contourArea)
        M = cv2.moments(largest_black_contour)
        if M["m00"] != 0:
            cx = int(M["m10"] / M["m00"])
            cy = int(M["m01"] / M["m00"])

            print("Position de l'objet : x = {} px , y = {} px".format(cx, cy))
            print("Position de l'objet : x = {} cm , y = {} cm".format((cx - coin_x) / rapport_x, (cy - coin_y) / rapport_y))

            cv2.drawContours(output, [largest_black_contour], -1, (0, 255, 0), 2)
            cv2.circle(output, (cx, cy), 5, (255, 0, 0), -1)
            cv2.imwrite('image_finale.png', output)
            return (cx - coin_x) / rapport_x, (cy - coin_y) / rapport_y
        else:
            print("Erreur : Calcul du centre du contour noir échoué")
            return None, None

def main():
    
    ser = serial.Serial('/dev/ttyACM0',9600)
    print("Connecté sur le port Arduino")
    
    # Initialiser la caméra
    cap = cv2.VideoCapture(0)

    if not cap.isOpened():
        print("Erreur : Impossible d'ouvrir la caméra")
        exit()

    # Donner du temps à la caméra pour la mise au point
    time.sleep(1)

    # Capture pour la calibration
    print("Phase de calibration...")
    gray_frame = capture_image(cap)
    cv2.imwrite('image_initiale_calibration.png', gray_frame)

    coin_x, coin_y, rapport_x, rapport_y = calibrate(gray_frame)
    if coin_x is None or coin_y is None:
        print("Erreur lors de la calibration.")
        cap.release()
        exit()

    # Attendre 2 minutes
    print("Attente de 1 minute avant la détection de l'objet...")
    time.sleep(60)

    # Capture pour la détection de l'objet
    print("Phase de détection de l'objet...")
    gray_frame = capture_image(cap)
    cv2.imwrite('image_initiale_detection.png', gray_frame)

    res_x, res_y = detect_object(gray_frame, coin_x, coin_y, rapport_x, rapport_y)
    if res_x is None or res_y is None:
        print("Erreur lors de la detection de l'objet.")
        cap.release()
        exit()
    
    position_message= f"C,{res_x:.2f},{res_y:.2f}\n"
    ser.write(position_message.encode())
    print("Positions envoyées à l'Arduino")
    
    ser.close()
    
    # Libérer les ressources
    cap.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    main()
