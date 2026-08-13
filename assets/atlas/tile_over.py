from PIL import Image, ImageDraw, ImageFont
from sys import argv

if __name__ == "__main__":
    # Validamos que se pasen los argumentos necesarios: script, imagen, ancho, alto
    if len(argv) < 4:
        print(f"Modo de uso:\npython tile_over.py imagen.png ancho_tile alto_tile")
        exit(-1)

    # Cargar imagen
    try:
        img = Image.open(argv[1])
    except Exception as e:
        print(f"Error al abrir la imagen: {e}")
        exit(-1)

    w, h = img.size
    print(f"Imagen original: {w}x{h} px")

    # Definir dimensiones de los tiles (pueden ser distintas)
    tilew = int(argv[2])
    tileh = int(argv[3])
    
    print(f"Procesando con tiles de: {tilew}x{tileh} px")

    # Crear objeto para dibujar
    draw = ImageDraw.Draw(img)

    # Intentar cargar una fuente estética, si no, usar la de sistema
    try:
        # Nota: Asegúrate que el archivo .ttf esté en la misma carpeta o usa path completo
        font = ImageFont.truetype("DejaVuSans-Bold.ttf", 20)
    except:
        font = ImageFont.load_default()

    count = 0
    # Iteramos en el eje Y (filas) saltando por el alto del tile
    for y in range(0, h, tileh):
        # Iteramos en el eje X (columnas) saltando por el ancho del tile
        for x in range(0, w, tilew):
            
            # Definimos los límites del rectángulo [x0, y0, x1, y1]
            # Usamos min() para no dibujar fuera de los bordes de la imagen
            box = [x, y, min(x + tilew, w), min(y + tileh, h)]
            
            # Dibujar el contorno del tile
            draw.rectangle(box, outline="red", width=1)
            
            # Escribir el índice del tile
            draw.text((x + 5, y + 5), str(count), fill="yellow", font=font)
            
            count += 1

    # Generar nombre de salida
    str_split = argv[1].rsplit(".", 1) # Usamos rsplit por si hay puntos en el nombre
    out_path = f"{str_split[0]}_tiled_{tilew}x{tileh}.{str_split[1]}"
    
    img.save(out_path)
    print(f"Proceso terminado. Imagen guardada como: {out_path}")
#from PIL import Image, ImageDraw, ImageFont
#from sys import argv
#if __name__ == "__main__":
#    if len(argv)< 3:
#        print(f"Modo de usarse (solo tiles cuadrados)\ntile_over.py imagen.png tile_dim")
#        exit(-1)
#    # Load image
#    img = Image.open(argv[1])
#    w, h = img.size
#
#    print(f"Imagen dim: {w}x{h}")
#
#    tilew = int(argv[2])
#    tileh = int(argv[3])
#    # Create overlay
#    draw = ImageDraw.Draw(img)
#
#    # Try a simple font
#    try:
#        font = ImageFont.truetype("DejaVuSans-Bold.ttf", 24)
#    except:
#        font = ImageFont.load_default()
#
#    count = 0
#    for y in range(0, h, tilew):
#        for x in range(0, w, tileh):
#            # rectangle
#            draw.rectangle([x, y, x+tilew, y+tileh], outline="red", width=1)
#            # number
#            draw.text((x+5, y+5), str(count), fill="yellow", font=font)
#            count += 1
#
#    str_split = argv[1].split(".")
#    out_path = str_split[0]+'_tiled.'+str_split[1]
#    img.save(out_path)
    out_path
