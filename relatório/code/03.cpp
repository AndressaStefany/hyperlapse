 Mat T = estimateRigidTransform(great_corners[1], great_corners[0], false);

 if(T.data==NULL){
   // identidade + coluna de zeros
   T = Mat(Size(3,2),CV_32FC1, Scalar(0));
   T(Range(0,2),Range(0,2))= Mat::eye(2,2,CV_32FC1);
 } else {
   // soma dos deslocamentos e angulo
   // pois o deslocamento eh com relacao ao frame anterior
   soma_dx += T.at<double>(0,2);
   soma_dy += T.at<double>(1,2);
   soma_da += atan2(T.at<double>(1,0), T.at<double>(0,0));
 }
 // atualiza prev_image
 prev_image = next_image.clone();
