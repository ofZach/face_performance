#include "shapemodel.h"
#include <cstdio>
#include "asmmodel.h"
#include "afreader.h"
using namespace cv;

ShapeModel::ShapeModel()
{
    pyramidLevel = 3;
}

void ShapeModel::loadShapeInfo(const char* shapeFileName)
{
    printf("Loading shape info from %s\n", shapeFileName);
    int nPathT;
    
    AFReader shapeDefFile(shapeFileName);
//     FILE *fp = fopen(shapeFileName, "r");
    FILE *fp = shapeDefFile.FH();
    shapeDefFile.Sync();
    fscanf(fp, "%d", &nMarkPoints);
    shapeDefFile.Sync();
    fscanf(fp, "%d", &nPathT);
    
    int i, j;
    shapeInfo.pathSeg.resize(nPathT+1);
    shapeInfo.pathType.resize(nPathT);
    shapeInfo.nPath = nPathT;
    shapeInfo.pathSeg[0] = 0;
    for (i=0;i<nPathT;i++){
        shapeDefFile.Sync();
        fscanf(fp, "%d%d", &shapeInfo.pathSeg[i+1], 
                        &shapeInfo.pathType[i]);
    }
    
    // r.y -= r.height*?
    shapeDefFile.Sync();
    fscanf(fp, "%lf", &searchYOffset);
    // r.x -= r.width*?
    shapeDefFile.Sync();
    fscanf(fp, "%lf", &searchXOffset);
    // r.width *= ?
    shapeDefFile.Sync();
    fscanf(fp, "%lf", &searchWScale);
    // r.height *= ?
    shapeDefFile.Sync();
    fscanf(fp, "%lf", &searchHScale);
    // step: ?*100/sqrt(area)
    shapeDefFile.Sync();
    fscanf(fp, "%lf", &searchStepAreaRatio);
    
    // init scale ratio when searching
    shapeDefFile.Sync();
    fscanf(fp, "%lf", &searchScaleRatio);
    // init X offset when searching
    shapeDefFile.Sync();
    fscanf(fp, "%lf", &searchInitXOffset);
    // init Y offset when searching
    shapeDefFile.Sync();
    fscanf(fp, "%lf", &searchInitYOffset);

    shapeInfo.pointInfo.resize(nMarkPoints);
    for (i=0;i<nPathT;i++){
        for (j=shapeInfo.pathSeg[i]; j<shapeInfo.pathSeg[i+1]; j++){
            shapeInfo.pointInfo[j].pathId   = i;
            shapeInfo.pointInfo[j].type     = shapeInfo.pathType[i];
            shapeInfo.pointInfo[j].connectFrom  = j-1;
            shapeInfo.pointInfo[j].connectTo    = j+1;
        }
        if (shapeInfo.pathType[i]==1){
            shapeInfo.pointInfo[shapeInfo.pathSeg[i]].connectFrom = shapeInfo.pathSeg[i+1]-1;
            shapeInfo.pointInfo[shapeInfo.pathSeg[i+1]-1].connectTo = shapeInfo.pathSeg[i];
        }
        else {
            shapeInfo.pointInfo[shapeInfo.pathSeg[i]].connectFrom = shapeInfo.pathSeg[i];
            shapeInfo.pointInfo[shapeInfo.pathSeg[i+1]-1].connectTo = shapeInfo.pathSeg[i+1]-1;
        }
    }
}

void ShapeModel::readTrainData(const char *listFileName)
{
	// Find the directory of the list file
	string sName(listFileName), listDir;
	int posD;
	posD = sName.find_last_of("/\\");
	if (posD != string::npos)
		listDir = sName.substr(0, posD+1);
	else
		listDir = "./";
	
    FILE *fp = fopen(listFileName,"r");
    if (fp==NULL){
        printf("ERROR! list file %s not found!!", listFileName);
        throw("ERROR! list file not found!!");
    }
    printf("Reading data from %s...\n", listFileName);
    ModelImage *ss;
    char sBuf[300];
    int l;
	string ptsPath;
    while (!feof(fp)){
        char * nk=fgets(sBuf,300,fp);
        l=strlen(sBuf);
        if (nk>0 && sBuf[l-1]=='\n')
            sBuf[l-1]=0;
        if (nk==0 || sBuf[0]==0)
            continue;
		if (sBuf[0]=='/')
			ptsPath = sBuf;
		else
			ptsPath = listDir + sBuf;
		
        ss = new ModelImage();
        ss->readPTS(ptsPath.data());
        ss->setShapeInfo( &shapeInfo );
//        ss->loadTrainImage();
//        ss->show();
        this->imageSet.push_back(*ss);
        delete ss;
    }
    this->nTrain = imageSet.size();
    this->nMarkPoints = imageSet[0].NPoints();
    fclose(fp);
}

void ShapeModel::buildModel()
{
    this->alignShapes();
    this->buildPCA();
    preparePatterns();
}

void ShapeModel::alignShapes()
{
    int i, nss;
    nss = imageSet.size();

    // Align
    for (i=0;i<nss;i++)
        imageSet[i].shapeVec.zeroGravity();

    ShapeVec curMean, newMean, x0;
    imageSet[0].shapeVec.scaleToOne();
    newMean = imageSet[0].shapeVec.clone();
    x0 = newMean.clone();

    do {
        curMean = newMean.clone();
        newMean = ShapeVec::zeros(curMean.size());
        for (i=0;i<nss;i++){
            imageSet[i].shapeVec.alignTo(curMean);
            newMean += imageSet[i].shapeVec;
        }
        newMean /= nss;
        newMean.alignTo(x0);
        newMean.scaleToOne();
    } while (norm(curMean-newMean)>1e-10);

    meanShape = curMean;
    // meanShape is not generated from any transformation, then scale=0
//    meanShape.setTransform(0, 0, 0);
}

void ShapeModel::buildPCA()
{
    // PCA
    int i, j;
    int vD = imageSet[0].shapeVec.rows;
    Mat_<double> pca_data;
    pca_data.create(vD, nTrain);
    for (i=0;i<nTrain;i++){
        for (j=0;j<vD;j++)
            pca_data(j, i) = (imageSet[i].shapeVec)(j, 0);
    }
    printf("(II) Calculating PCA of shape vectors.\n");
    pcaShape = new PCA(pca_data, Mat_<double>(), CV_PCA_DATA_AS_COL, 0);
    double eigValueSum, sCur;
    eigValueSum = cv::sum(pcaShape->eigenvalues)[0];
    sCur = 0;
    printf("(II) PCA Rows: %d, Var: %lf\n",
           pcaShape->eigenvalues.rows, eigValueSum);

//     printf("Total Data: %d\n", pcaShape->eigenvalues.rows);
//     for (i=0; i<pcaShape->eigenvalues.rows; i++){
//         printf("%d: %g\n", i, pcaShape->eigenvalues.at<double>(i, 0));
//     }
           
    
           
    for (i=0;i<pcaShape->eigenvalues.rows && i<40;i++){
        sCur += pcaShape->eigenvalues.at<double>(i, 0);
//         printf("%d: %g\n", i, pcaShape->eigenvalues.at<double>(i, 0));
        if (sCur>eigValueSum*0.98)
            break;
    }
    
    // Prepare for the BTSM
    this->sigma2 = (eigValueSum - sCur)/(vD - 4);
    printf("sssiggg: %g\n", sigma2);
    this->pcaFullShape = new PCA();
    pcaFullShape->eigenvalues = pcaShape->eigenvalues.clone();
    pcaFullShape->eigenvectors = pcaShape->eigenvectors.clone();
    pcaFullShape->mean = pcaShape->mean.clone();
    
    if (i<pcaShape->eigenvalues.rows)
        nShapeParams = i+1;
    else
        nShapeParams = i;
    pcaShape->eigenvalues = pcaShape->eigenvalues.rowRange(0, nShapeParams);
    pcaShape->eigenvectors = pcaShape->eigenvectors.rowRange(0, nShapeParams);
    printf("(II) Shape Model: reserved parameters:%d, variance: %.2f%%\n", nShapeParams, 100*sCur/eigValueSum);
}

void ShapeModel::preparePatterns()
{
    // Fat or thin? determined by width/height
    cv::Rect_<double> r;
    double ratio;
    Mat_<double> cParam, pt;
    Mat_<double> sumParam;
    double sumW;
    
    sumW = 0;
    sumParam = Mat_<double>::zeros(nShapeParams, 1);
    
    for (int i=0; i<nTrain; i++){
        r = imageSet[i].shapeVec.getBoundRect();
        projectShapeToParam(imageSet[i].shapeVec, cParam);
        ratio = r.width / r.height;
//         printf("%f\n", ratio);
        if (ratio>0.7){
            pt = cParam * (ratio-0.7);
            sumParam += pt;
            sumW += ratio-0.7;
        }
    }
    sumParam /= sumW;
    sumParam = normalizeParam(sumParam);
    printf("Fat & Thin: ");
    for (int i=0; i<nShapeParams; i++)
        printf("%g, ", sumParam(i, 0));
    printf("\n");
    
    // Eye Size
    sumW = 0;
    sumParam = Mat_<double>::zeros(nShapeParams, 1);
    Point_<double> v;
    double l1, l2;
    for (int i=0; i<nTrain; i++){
        v = (imageSet[i].points[27]-imageSet[i].points[29]);
        l1 = v.dot(v);
        v = (imageSet[i].points[28]-imageSet[i].points[30]);
        l2 = v.dot(v);
        ratio = sqrt(l1*l2);
        projectShapeToParam(imageSet[i].shapeVec, cParam);
        printf("%f\n", ratio);
        if (ratio>350){
            pt = cParam * (ratio-350);
            sumParam += pt;
            sumW += ratio-350;
        }
    }
    sumParam /= sumW;
    sumParam = normalizeParam(sumParam);
    printf("eyeSize: ");
    for (int i=0; i<nShapeParams; i++)
        printf("%g, ", sumParam(i, 0));
    printf("\n");
}

void ShapeModel::saveToFile(ModelFile &file)
{
    file.writeInt(pyramidLevel);
    file.writeInt(nMarkPoints);
    file.writeInt(nTrain);
    file.writeInt(nShapeParams);
    
    file.writeReal(searchYOffset);
    file.writeReal(searchXOffset);
    file.writeReal(searchWScale);
    file.writeReal(searchHScale);
    file.writeReal(searchStepAreaRatio);
    
    file.writeReal(searchScaleRatio);
    file.writeReal(searchInitXOffset);
    file.writeReal(searchInitYOffset);
    
    file.writePCA(pcaShape);
    for (int i=0;i<nMarkPoints*2;i++)
        file.writeReal(meanShape(i, 0));
    
    // Info for BTSM
    file.writeReal(sigma2);
    file.writePCA(pcaFullShape);
    
    shapeInfo.writeToFile(file);

    ////! Mean shape after aligning
    //ShapeVec meanShape;
}

void ShapeModel::loadFromFile(ModelFile &file)
{
    printf("Loading Shape model from file...\n");
    file.readInt(pyramidLevel);
    file.readInt(nMarkPoints);
    file.readInt(nTrain);
    file.readInt(nShapeParams);

    file.readReal(searchYOffset);
    file.readReal(searchXOffset);
    file.readReal(searchWScale);
    file.readReal(searchHScale);
    file.readReal(searchStepAreaRatio);
    
    file.readReal(searchScaleRatio);
    file.readReal(searchInitXOffset);
    file.readReal(searchInitYOffset);
    
    // PCA shape model
    file.readPCA(pcaShape);

    meanShape.create(nMarkPoints*2, 1);
    for (int i=0;i<nMarkPoints*2;i++)
        file.readReal(meanShape(i, 0));
    
    // Info for BTSM
    file.readReal(sigma2);
    file.readPCA(pcaFullShape);
    
    shapeInfo.readFromFile(file);
}

void ShapeModel::load(const string& path, bool bin)
{
    if (bin){
        ModelFile mf;
        mf.openFile(path.data(), "rb");
        this->loadFromFile(mf);
        mf.closeFile();
    }
    else {
        ModelFileAscii mf;
        mf.openFile(path.data(), "r");
        this->loadFromFile(mf);
        mf.closeFile();
    }
}

void ShapeModel::save(const string& path, bool bin)
{
    if (bin){
        ModelFile mf;
        mf.openFile(path.data(), "wb");
        this->saveToFile(mf);
        mf.closeFile();
    }
    else {
        ModelFileAscii mf;
        mf.openFile(path.data(), "w");
        this->saveToFile(mf);
        mf.closeFile();
    }
}

void ShapeModel::projectParamToShape(const Mat_<double> & paramVec,
                                     ShapeVec & shapeVec)
{
    this->pcaShape->backProject(paramVec, shapeVec);
}

void ShapeModel::projectShapeToParam(const ShapeVec & shapeVec,
                                     Mat_<double> & paramVec)
{
    this->pcaShape->project(shapeVec, paramVec);
}

void ShapeModel::getShapeParam(ModelImage &mi, FitResult &res)
{
    mi.shapeVec.zeroGravity();
    mi.shapeVec.scaleToOne();
    mi.shapeVec.alignTo(this->meanShape);
    
    FitResult ans;
    projectShapeToParam(mi.shapeVec, ans.params);
    res = ans;
}

void ShapeModel::clampParamVec( Mat_< double > &paramVec )
{
    for (int i=0;i<this->nShapeParams;i++){
        double ei = sqrt(pcaShape->eigenvalues.at<double>(i, 0));
        if (paramVec(i, 0) > 3*ei)
            paramVec(i, 0) = 3*ei;
        else if (paramVec(i, 0) < -3*ei)
            paramVec(i, 0) = -3*ei;
    }
}

#include "highgui.h"
using cv::namedWindow;
using cv::createTrackbar;
using cv::setTrackbarPos;

void viewShapeUpdateValue(int pos, void *data)
{
    ModelViewInfo *pInfo = (ModelViewInfo *)data;
    pInfo->vList[pInfo->curParam] = pos;
    ((ShapeModel *)(pInfo->pModel))->viewShapeModelUpdate(pInfo);
}

void viewShapeUpdateCurParam(int pos, void *data)
{
    ModelViewInfo *pInfo = (ModelViewInfo *)data;
    pInfo->curParam = pos;
    cvSetTrackbarPos("param value", "Viewing Shape Model", 
                   pInfo->vList[pos]);
}

void ShapeModel::viewShapeModelUpdate(ModelViewInfo *pInfo)
{
    Mat_< double > paramV;
    paramV.create(this->nShapeParams, 1);
    for (int i=0;i<nShapeParams;i++){
        paramV(i, 0) = (pInfo->vList[i]/30.0 - 0.5)*6*
                        sqrt(pcaShape->eigenvalues.at<double>(i, 0));
    }
    Mat_<cv::Vec3b> img;
    
    ModelImage s;
    s.setShapeInfo(&shapeInfo);
    s.loadTrainImage(Mat_<unsigned char>::ones(190*2, 160*2)*255);
    projectParamToShape(paramV, s.shapeVec);
    SimilarityTrans st = s.shapeVec.getShapeTransformFitingSize(
                            cv::Size(320, 380));
    s.buildFromShapeVec(st);
    img = s.show(0, -1, false);
    imshow("Viewing Shape Model", img);
}

void ShapeModel::viewShapeModel()
{
    int q1, q2;
    static ModelViewInfo vData;
    vData.vList.resize(this->nShapeParams, 30/2);
    vData.pModel = this;
    vData.curParam = 0;
    viewShapeModelUpdate(&vData);
    q1 = 15;
    q2 = 0;
    namedWindow("Viewing Shape Model", CV_WINDOW_AUTOSIZE);
    createTrackbar("param value", "Viewing Shape Model", 
                   &q1, 30, &viewShapeUpdateValue, &vData);
    createTrackbar("which param", "Viewing Shape Model", 
                   &q2, nShapeParams-1, &viewShapeUpdateCurParam, &vData);
}
