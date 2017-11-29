 // atualiza deslocamentos e angulo
 dx= soma_dx;
 dy= soma_dy;
 da= soma_da;

 // recalcula a matriz T
 T.at<double>(0,0) = cos(da);
 T.at<double>(0,1) = -sin(da);
 T.at<double>(1,0) = sin(da);
 T.at<double>(1,1) = cos(da);

 T.at<double>(0,2) = dx;
 T.at<double>(1,2) = dy;

 // aplica a rotacao e translado
 warpAffine(next_color_image, image_new, T, next_color_image.size());
