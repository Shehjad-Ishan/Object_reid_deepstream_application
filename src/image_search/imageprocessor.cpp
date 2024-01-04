#include "imageprocessor.h"
#include <iostream>
#include <QDebug>
#include <QImage>
#include <QByteArray>
#include <QFile>
#include <QTextStream>
#include <QBuffer>
#include <faiss/IndexFlat.h>
#include <faiss/index_io.h>
#include <faiss/Index.h>
#include <cstring>
#include <sqlite3.h>

ImageProcessor::ImageProcessor(QObject *parent) : QObject(parent)
{
}

void ImageProcessor::processImage(int imageId)
{

    // Connect to SQLite3 database
    sqlite3 *db;
    if (sqlite3_open("/home/sigmind/Downloads/deepstream-6.3/sources/apps/sample_apps/deepstream-app/reidt.db", &db) != SQLITE_OK) {
       qDebug() << "Error opening SQLite3 database";
       return;
    }
    // Specify the dimensionality of the feature vectors
    int embedding_dim = 256;

    // Create a Faiss index
    faiss::IndexFlatL2 index(embedding_dim);


    // Query SQLite3 to retrieve all blobs
    const char *query1 = "SELECT EMBEDDING FROM FACE";
    sqlite3_stmt *stmt;
    if (sqlite3_prepare_v2(db, query1, -1, &stmt, nullptr) == SQLITE_OK) {
       while (sqlite3_step(stmt) == SQLITE_ROW) {
             const void *blobData = sqlite3_column_blob(stmt, 0);
             int blobSize = sqlite3_column_bytes(stmt, 0);

             // Convert the blob data to a vector of floats
             std::vector<float> featureVector(blobSize / sizeof(float));
             std::memcpy(featureVector.data(), blobData, blobSize);

             // Add the feature vector to the Faiss index
             index.add(1, featureVector.data());
     }


       // Print the contents of the index
      /* for (int i = 0; i < 400; i++) {
           std::vector<float> featureVector(embedding_dim);
           index.reconstruct(i, featureVector.data());
           std::cout << "Index: " << i << "  Feature vector: ";
           for (float value : featureVector) {
               std::cout << value << " ";
           }
           std::cout << std::endl;
       }*/


            // Finalize the prepared statement
            sqlite3_finalize(stmt);


        } else {
            qDebug() << "Error preparing SQL statement: " << sqlite3_errmsg(db);
        }
    /*Debug the faiss index
    // Assuming 'index' is your Faiss index
    int n = index.ntotal;

    // Allocate memory to hold the vectors
    std::vector<float> vectors(n * index.d);

    // Copy the vectors from the index to the allocated memory
    index.reconstruct(0, vectors.data());

    // Print the vectors
    for (int i = 0; i < n; ++i) {
        qDebug() << "Vector " << i << ":";
        for (int j = 0; j < index.d; ++j) {
            qDebug() << vectors[i * index.d + j];
        }
        qDebug() << "--------------------------";
    } */

    // Query SQLite3 to retrieve blob associated with image_id
    const char *query = "SELECT EMBEDDING FROM FACE WHERE IMG_ID = ?";

    if (sqlite3_prepare_v2(db, query, -1, &stmt, nullptr) == SQLITE_OK) {
        sqlite3_bind_int(stmt, 1, imageId);

        int stepResult = sqlite3_step(stmt);

        if (stepResult == SQLITE_ROW) {
            const void *blobData = sqlite3_column_blob(stmt, 0);
            int blobSize = sqlite3_column_bytes(stmt, 0);


            // Convert the blob data to a vector of floats
            std::vector<float> featureVector(blobSize / sizeof(float));
            std::memcpy(featureVector.data(), blobData, blobSize);

            // Add the feature vector to the Faiss index

            //index.add(1, featureVector.data());

            // Continue with the Faiss search...
            // Convert to float array
            //std::vector<float> floatData(dataSize / sizeof(float));
            //std::memcpy(floatData.data(), imageData, dataSize);


            // Print floatData
            /*qDebug() << "Float Data:";
            for (float value : floatData) {
                qDebug() << value;

            }*/


            // Perform similarity search using Faiss
            int k = 50; // Number of similar vectors to search for
            faiss::idx_t* I = new faiss::idx_t[k];
            float* D = new float[k];

            index.search(1, featureVector.data(), k, D, I);

            // Display or print the results
            qDebug() << "Similar images:";
            QFile outputFile("/home/sigmind/Downloads/deepstream-6.3/sources/apps/sample_apps/deepstream-app/src/image_search/output.txt");
            if (outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream outputStream(&outputFile);
                for (int i = 0; i < k; ++i) {
                    qDebug() << "Index:" << I[i] << " Distance:" << D[i];
                    outputStream << "Index:" << I[i] << " Distance:" << D[i] << "\n";
                    // Additional processing or output based on search results
                }
                // Close the file
                outputFile.close();
                qDebug() << "Output written to file: /home/sigmind/Downloads/deepstream-6.3/sources/apps/sample_apps/deepstream-app/src/image_search/output.txt";
            } else {
                qDebug() << "Error opening output file";
            }

            // Cleanup
            delete[] I;
            delete[] D;
        } else {
            qDebug() << "No rows found for image_id" << imageId;
        }

        // Finalize the prepared statement
        sqlite3_finalize(stmt);
    }

    else {

        qDebug() << "Error preparing SQL statement: " << sqlite3_errmsg(db);

         }

    // Close the SQLite3 database connection
    sqlite3_close(db);
}



