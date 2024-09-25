import cv2
import numpy as np
import time 

# Calibration
coin_x = 291
coin_y = 551
rapport_x = 29.51666
rapport_y = 29.3666 

# Initialiser la caméra
cap = cv2.VideoCapture(0)

# Vérifier si la caméra s'est bien ouverte
if not cap.isOpened():
    print("Erreur : Impossible d'ouvrir la caméra")
    exit()

# Donner du temps a la camera pour la mise au point 
time.sleep(1)

# Capturer une image
ret, frame = cap.read()

# Vérifier si la capture a réussi
if not ret:
    print("Erreur : Échec de la capture d'image")
    cap.release()
    exit()

# Convertir l'image en niveaux de gris
gray_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
cv2.imwrite('image initiale.png', gray_frame)

# Définir une plage de seuils pour détecter le rectangle blanc
_, white_mask = cv2.threshold(gray_frame, 100, 120, cv2.THRESH_BINARY)

# Trouver les contours du rectangle blanc
contours, _ = cv2.findContours(white_mask, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)

# Vérifier si des contours ont été trouvés
if len(contours) == 0:
    print("Aucun rectangle blanc détecté")
    cap.release()
    exit()

# Trouver le plus grand contour (en supposant que c'est le rectangle blanc)
largest_contour = max(contours, key=cv2.contourArea)

# Créer un masque pour le rectangle blanc
mask = np.zeros_like(gray_frame)
cv2.drawContours(mask, [largest_contour], -1, 255, thickness=cv2.FILLED)
cv2.imwrite("Masque Rectangle Blanc.png", mask)

# Appliquer le masque pour ne garder que la région à l'intérieur du rectangle blanc
roi = cv2.bitwise_and(gray_frame, gray_frame, mask=mask)

# Sauvegarder l'image du masque pour visualisation
cv2.imwrite('roi.png', roi)

# Définir une plage de seuils pour détecter les zones noires dans la ROI
_, black_mask = cv2.threshold(roi, 50, 255, cv2.THRESH_BINARY_INV)

# Sauvegarder l'image du masque des zones noires pour visualisation
cv2.imwrite('black_mask_within_white_rectangle.png', black_mask)

may=cv2.bitwise_and(black_mask,mask)
cv2.imwrite('may.png', may)

# Trouver les contours des zones noires dans la ROI
black_contours, _ = cv2.findContours(may, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
output=cv2.cvtColor(may, cv2.COLOR_GRAY2BGR)

# Vérifier si des contours ont été trouvés
if len(black_contours) == 0:
    print("Aucune zone noire détectée à l'intérieur du rectangle blanc")
else:
    # Trouver le plus grand contour (en supposant que c'est l'objet d'intérêt)
    largest_black_contour = max(black_contours, key=cv2.contourArea)
    
    # Calculer le centre du contour
    M = cv2.moments(largest_black_contour)
    if M["m00"] != 0:
        cx = int(M["m10"] / M["m00"])
        cy = int(M["m01"] / M["m00"])
        
        print("Position de l'objet : x = {} px , y = {} px".format(cx, cy))
        print("Position de l'objet : x = {} cm , y = {} cm".format((cx-coin_x)/rapport_x, (cy-coin_y)/rapport_y))
        
        # Dessiner le contour et le centre sur l'image pour vérification
        cv2.drawContours(output, [largest_black_contour], -1, (0, 255, 0), 2)
        cv2.circle(output, (cx, cy), 5, (255, 0, 0), -1)

        # Enregistrer l'image annotée
        cv2.imwrite('image_finale.png', output)
    else:
        print("Erreur : Calcul du centre du contour noir échoué")

# Libérer les ressources
cap.release()
cv2.destroyAllWindows()

