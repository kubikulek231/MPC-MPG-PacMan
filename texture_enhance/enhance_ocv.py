import cv2
import numpy as np

img = cv2.imread("stars_3.jpg", cv2.IMREAD_COLOR)

# Get the image dimensions
height, width, _ = img.shape

# Define the size of the square crop
crop_size = 5000

# Calculate the center of the image
center_x, center_y = width // 2, height // 2

# Calculate the cropping box to get the square region
x1 = max(center_x - crop_size // 2, 0)
x2 = min(center_x + crop_size // 2, width)
y1 = max(center_y - crop_size // 2, 0)
y2 = min(center_y + crop_size // 2, height)

# Crop the image to a square
cropped_img = img[y1:y2, x1:x2]

# Gamma correction (boost the dark areas without overexposing the bright parts)
gamma = 1.5
gamma_corrected_img = np.zeros_like(cropped_img)
for i in range(3):  # Apply gamma correction to each channel
    gamma_corrected_img[:, :, i] = np.power(cropped_img[:, :, i] / 255.0, gamma) * 255.0

# Convert to uint8 after gamma correction
output = gamma_corrected_img.astype(np.uint8)

# Add warm tones (red, yellow) by boosting red and green channels
output[:, :, 1] = np.clip(output[:, :, 1] * 1.15, 0, 255)  # Slightly boost green
output[:, :, 2] = np.clip(output[:, :, 2] * 1.25, 0, 255)  # Slightly boost red

# Save result
cv2.imwrite("stars_enhanced.jpg", output)
cv2.imwrite("stars_texture.png", output)