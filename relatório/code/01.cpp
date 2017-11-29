 VideoCapture cap("media/video.mp4");	
 cap>>prev_image;

 prev_color_image = prev_image.clone();

 cvtColor(prev_image, prev_image, COLOR_BGR2GRAY);


