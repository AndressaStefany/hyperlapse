 cap>>next_image;

 goodFeaturesToTrack(prev_image, corners[0], 300, 0.01, 20, mask, 3, false, 0.01);
 calcOpticalFlowPyrLK(prev_image, next_image, corners[0], corners[1], status, err, winSize, 3, termcrit, 0, 0.001);

 for(size_t i=0; i < status.size(); i++) {
   if(status[i]) { // tira as bordas ruins
       greatCorners[0].push_back(corners[0][i]);
       greatCorners[1].push_back(corners[1][i]);
   }// fim do if
 }// fim do for

