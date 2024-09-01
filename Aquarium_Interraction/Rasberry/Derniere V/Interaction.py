import cv2
import mediapipe as mp
import numpy as np

# Initialisation de MediaPipe Pose
mp_pose = mp.solutions.pose
pose = mp_pose.Pose(static_image_mode=False, min_detection_confidence=0.5, min_tracking_confidence=0.5)
mp_drawing = mp.solutions.drawing_utils

# Initialisation de la capture vidéo
cap = cv2.VideoCapture(0)  # '0' pour la caméra par défaut, ou spécifiez le périphérique de la caméra si nécessaire

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        print("Erreur de capture vidéo")
        break

    # Convertir l'image en RGB
    image_rgb = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)

    # Traitement de l'image pour obtenir les landmarks
    results = pose.process(image_rgb)

    # Vérifier si des landmarks ont été détectés
    if results.pose_landmarks:
        # Dessiner les landmarks sur l'image
        mp_drawing.draw_landmarks(frame, results.pose_landmarks, mp_pose.POSE_CONNECTIONS)

        # Extraire les coordonnées (x, y, z) des landmarks
        for id, landmark in enumerate(results.pose_landmarks.landmark):
            h, w, _ = frame.shape
            x, y, z = int(landmark.x * w), int(landmark.y * h), landmark.z

            # Afficher les coordonnées sur l'image
            cv2.putText(frame, f'ID: {id} X: {x} Y: {y} Z: {z:.2f}', (x, y), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 255, 0), 2)

    # Afficher l'image
    cv2.imshow('MediaPipe Pose', frame)

    # Quitter la boucle en appuyant sur 'q'
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Libérer les ressources
cap.release()
cv2.destroyAllWindows()

