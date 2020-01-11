#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core.hpp>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>

#define w 400

using namespace std;
using namespace cv;


int main( int argc, char** argv )
{
Mat imageSource, imageMedian, imageModifiee, imageEnNiveau2Gris, imageOtsu, elementStructurantD, elementStructurantE, imageDilatee, imageErodee, imageSegmentee,imagePostsegmentee, imageContrastee ;
char* imageOriginal = argv[1];
char image_window[] = "PicEdited";
Mat image = Mat::zeros( w, w, CV_8UC3 );
int MAX_KERNEL_LENGTH = 2;

    

//Lecture de l'image
image = imread( imageOriginal, IMREAD_COLOR );
if( argc != 2 || !image.data )
{
  printf( " No image data \n " );
  return -1;
}
namedWindow( imageOriginal, WINDOW_AUTOSIZE );
imshow( imageOriginal, image );
  
//Phase de pré-traitement
  //Modification de la taille de l'image
  Size size(350,350);
  resize(image,imageModifiee,size);
	  //Enregistrement et affichage de l'image modifiée
	  namedWindow( "Image Modifiée", WINDOW_AUTOSIZE );
	  imwrite( "Image Modifiée.jpg", imageModifiee );
	  imshow("Image Modifiée", imageModifiee );
  
  //application de l'algoritme du contraste à l'image
  imageContrastee = Mat::zeros( imageModifiee.size(), imageModifiee.type() );
     for( int y = 0; y < imageModifiee.rows; y++ ) {
        for( int x = 0; x < imageModifiee.cols; x++ ) {
            for( int z = 0; z < imageModifiee.channels(); z++ ) {
                imageContrastee.at<Vec3b>(y,x)[z] =
                  saturate_cast<uchar>( 1.3*imageModifiee.at<Vec3b>(y,x)[z] + 40 );
            }
        }
    }
	    //Enregistrement et affichage de l'image contrastée
	    imwrite( "Image Contrastée.jpg", imageContrastee );
	    imshow("Image Contrastée", imageContrastee);
  
  moveWindow( image_window, w, 300 );

  //Application du filtre Median
  for ( int i = 1; i < MAX_KERNEL_LENGTH; i = i + 2 )
         { medianBlur ( imageContrastee, imageMedian, i );}
  //Enregistrement et affichage de l'image Median
  imwrite( "Image Median.jpg", imageMedian );
  namedWindow( "Application du Filtre Median", CV_WINDOW_AUTOSIZE );
  imshow( "Application du Filtre Median", imageMedian );  
  
//Phase de segmentation
  //Conversion de l'image couleur en image en niveaux de gris
     cvtColor(imageMedian, imageEnNiveau2Gris, COLOR_RGB2GRAY);
	     //Enregistrement et affichage de l'image enn niveau de gris
	     namedWindow( "Image Niveau de Gris", CV_WINDOW_AUTOSIZE );
	     imwrite( "Image Niveau de Gris.jpg", imageEnNiveau2Gris );
	     imshow("Image Niveau de Gris", imageEnNiveau2Gris); 

  //Application de l'algorithme d'OTSU
    threshold(imageEnNiveau2Gris, imageOtsu, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
    	     //Enregistrement et affichage de l'image segmentée
	     namedWindow( "Image segmentée", CV_WINDOW_AUTOSIZE );
	     imwrite( "Image segmentée.jpg", imageOtsu );
             imshow("Image segmentée", imageOtsu);

//Début de la phase de post-segmentation
  //Définition de l'élément structurant
     elementStructurantD = getStructuringElement(MORPH_RECT, Size(10, 10));
     elementStructurantE = getStructuringElement(MORPH_RECT, Size(7, 7));

  //Dilatation de l'image
     dilate(imageOtsu, imageDilatee, elementStructurantD);
     	     //Enregistrement et affichage de l'image dilatée
	     namedWindow( "Image dilatée", CV_WINDOW_AUTOSIZE );
	     imwrite( "Image dilatée.jpg", imageDilatee );
     	     imshow("Image dilatée", imageDilatee);

  //Erosion de l'image
     erode(imageDilatee, imageErodee, elementStructurantE);
     	     //Enregistrement et affichage de l'image erodée
	     namedWindow( "Image erodée", CV_WINDOW_AUTOSIZE );
	     imwrite( "Image erodée.jpg", imageErodee );
     	     imshow("Image erodée", imageErodee);

  //Recherche des contours des régions détectées
     vector<vector<Point> > contours;
     findContours(imageErodee, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE);

  //Etiquetage des régions
     Mat markers = Mat::zeros(imageDilatee.size(), CV_32SC1);
     for (size_t i = 0; i < contours.size(); i++)
     drawContours(markers, contours, static_cast<int>(i), Scalar::all(static_cast<int>(i) + 1), -1);

  //Génération aléatoire de couleurs
        vector<Vec3b> colors;
        for (size_t i = 0; i < contours.size(); i++) {
            int b = theRNG().uniform(0, 255);
            int g = theRNG().uniform(0, 255);
            int r = theRNG().uniform(0, 255);
            colors.push_back(Vec3b((uchar) b, (uchar) g, (uchar) r));
        }

  //Coloration des régions avec les couleurs générées
        imagePostsegmentee = Mat::zeros(markers.size(), CV_8UC3);
        for (int i = 0; i < markers.rows; i++) {
            for (int j = 0; j < markers.cols; j++) {
                int index = markers.at<int>(i, j);
                if (index > 0 && index <= static_cast<int>(contours.size()))
                    imagePostsegmentee.at<Vec3b>(i, j) = colors[index - 1];
                else
                    imagePostsegmentee.at<Vec3b>(i, j) = Vec3b(0, 0, 0);
            }
        }

        imageMedian.copyTo(imageSegmentee, imagePostsegmentee);
 
 	     //Enregistrement et affichage de l'image obtenue après la post-segmentation
	     namedWindow( "Image Post-Segmentation", CV_WINDOW_AUTOSIZE );
	     imwrite( "Image Post-Segmentation.jpg", imagePostsegmentee );
             imshow("Image Post-Segmentation", imagePostsegmentee);
 waitKey(0);
 return 0;
}




