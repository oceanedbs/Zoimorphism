import cv2
import mediapipe as mp
import serial
import time

# Initialiser les modules MediaPipe
mp_pose = mp.solutions.pose
pose = mp_pose.Pose()
mp_drawing = mp.solutions.drawing_utils

# Configurer la connexion série
ser = serial.Serial('/dev/ttyACM0', 9600)  # Ajuster le port et la vitesse de transmission
time.sleep(2)  # Attendre que la connexion série soit prête

# Ouvrir la caméra
cap = cv2.VideoCapture(0)

# Variables pour suivre les coordonnées précédentes
previous_x, previous_z = None, None
previous_time = time.time()

# Plages de mouvement du danseur (normalisées entre 0 et 1)
min_x, max_x = 0.0, 1.0
min_z, max_z = 0.0, 1.0

# Plages de pas du moteur pour chaque axe
min_motor_steps_x, max_motor_steps_x = 0, 3125  # Plage de pas en X (ajuster selon le moteur)
min_motor_steps_z, max_motor_steps_z = 0, 625    # Plage de pas en Z (ajuster selon le moteur)

# Seuil d'amplitude en x et en z
threshold_x = 0.07
threshold_z = 0.07

# Compteur d'infos recus 
infos_recues=0

def map_to_motor_range(value, min_value, max_value, min_motor, max_motor):
    """Mapper une valeur normalisée à une plage de pas du moteur."""
    return int(min_motor + (value - min_value) / (max_value - min_value) * (max_motor - min_motor))

def send_motor_command(dx, dz, dt):
    # Mapper les changements du danseur en pas du moteur pour chaque axe
    motor_steps_x = map_to_motor_range(dx, -1.0, 1.0, min_motor_steps_x, max_motor_steps_x)
    motor_steps_z = map_to_motor_range(dz, -1.0, 1.0, min_motor_steps_z, max_motor_steps_z)

    # Envoyer les commandes au moteur
    if motor_steps_x != max_motor_steps_x/2 :
        command_x = 'DROITE' if motor_steps_x < max_motor_steps_x/2 else 'GAUCHE'
        direction_x = 'P' if motor_steps_x < max_motor_steps_x/2 else 'M'
        steps_x = abs(motor_steps_x)
        #print(f'Commande envoyée : {command_x} {steps_x}\n')
        
        if dt > 0.8: 
            command_dt = 'L'
        if dt<0.2 :
            command_dt = 'V'
        if dt<0.8 and dt>0.2:
            command_dt = 'M'
        #print(f'Commande envoyée : {command_dt} \n')
    
    if motor_steps_z != max_motor_steps_z/2:
        command_z = 'DEVANT' if motor_steps_z < max_motor_steps_z/2 else 'DERRIERE'
        direction_z = 'P' if motor_steps_z < max_motor_steps_z/2 else 'M'
        steps_z = abs(motor_steps_z)
        # ~ ser.write(f'{command_z} {steps_z}\n'.encode())
        #print(f'Commande envoyée : {command_z} {steps_z}\n')
        
        if dt > 0.8: 
            command_dt = 'L'
        if dt<0.2 :
            command_dt = 'V'
        if dt<0.8 and dt > 0.2:
            command_dt = 'M'
        #print(f'Commande envoyée : {command_dt} \n')
        
    ser.write(f'I,{direction_x},{abs(motor_steps_x)},{direction_z},{abs(motor_steps_z)},{command_dt}\n'.encode())
    print(f'I,{direction_x},{abs(motor_steps_x)},{direction_z},{abs(motor_steps_z)},{command_dt}\n')
    

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        break

    # Convertir l'image en RGB
    rgb_frame = cv2.cvtColor(frame, cv2.COLOR_BGR2RGB)
    results = pose.process(rgb_frame)

    if results.pose_landmarks:
        # Dessiner les points de repère
        mp_drawing.draw_landmarks(frame, results.pose_landmarks, mp_pose.POSE_CONNECTIONS)

        # Extraire les coordonnées des epaules du danseur 
        landmarks=results.pose_landmarks.landmark
        total_x, total_z=0,0
        num_landmarks=len(landmarks)
        
        for landmark in lanmarks:
            total_x += landmark.x
            total_z += landmark.z
        
        avg_x= total_x/num_landmarks
        avg_z= total_z/num_landmarks
        
        # Temps
        current_time = time.time()
        
        # Afficher les coordonnées de l'epaule gauche sur l'image 
        cv2.putText(frame, f'Epaules moyenne: x={avg_x:.2f}, z={avg_z:.2f}',(10,30), cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0,255,0),2)
        #print(f"Epaule gauche X: {x}, Z: {z}")

        if previous_x is not None and previous_z is not None :
            # Calculer les changements en coordonnées
            dx = avg_x - previous_x
            dz = avg_z - previous_z
            
            if abs(dx)>threshold_x or abs(dz)>threshold_z:
                
                infos_recues+=1
                
                # Calculer le temps écoulé
                dt = current_time - previous_time
                #print('dt=',dt)
                
                # Envoi des commandes aux moteurs toutes les 5 itérations
                if infos_recues>=5:
                    # Mapper les changements aux commandes du moteur
                    send_motor_command(dx, dz, dt)
                    print("Info envoyée")
                    infos_recues=0 
                
                
                # Délai d'une seconde après l'envoi de la commande 
                # time.sleep(1)
                
                # Mettre à jour le temps 
                previous_time = current_time

            # Mettre à jour les coordonnées précédentes
            previous_x, previous_z = avg_x, avg_z

    # Agrandissement de l'image
    frame=cv2.resize(frame, (1280,960))
    # Afficher l'image avec les points de repère
    cv2.imshow('Dance Tracking All Body', frame)

    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

# Libérer les ressources
cap.release()
cv2.destroyAllWindows()
ser.close()
