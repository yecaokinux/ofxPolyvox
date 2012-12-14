#include "ofxPolyvox.h"



ofxPolyvox::ofxPolyvox()
{
    //ctor
}

ofxPolyvox::~ofxPolyvox()
{
    //dtor
}

void ofxPolyvox::setSurfaceMeshToRender(const PolyVox::SurfaceMesh<PositionMaterialNormal>& surfaceMesh)
{

	//Convienient access to the vertices and indices
	const vector<uint32_t>& vecIndices = surfaceMesh.getIndices();
	const vector<PositionMaterialNormal>& vecVertices = surfaceMesh.getVertices();

	//Build an OpenGL index buffer
	glGenBuffers(1, &indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);
	const GLvoid* pIndices = static_cast<const GLvoid*>(&(vecIndices[0]));
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, vecIndices.size() * sizeof(uint32_t), pIndices, GL_STATIC_DRAW);

	//Build an OpenGL vertex buffer
	glGenBuffers(1, &vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	const GLvoid* pVertices = static_cast<const GLvoid*>(&(vecVertices[0]));
	glBufferData(GL_ARRAY_BUFFER, vecVertices.size() * sizeof(PositionMaterialNormal), pVertices, GL_STATIC_DRAW);

	m_uBeginIndex = 0;
	m_uEndIndex = vecIndices.size();
}

void ofxPolyvox::extractSurface( PolyVox::SurfaceMesh<PositionMaterialNormal>& surfaceMesh, SimpleVolume<uint8_t> volData){

    //We'll be rendering with index/vertex arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);

	CubicSurfaceExtractorWithNormals< SimpleVolume<uint8_t> > surfaceExtractor(&volData, volData.getEnclosingRegion(), &surfaceMesh);
	surfaceExtractor.execute();




}

void ofxPolyvox::polyvoxToOfMesh(const PolyVox::SurfaceMesh<PositionMaterialNormal>& surfaceMesh, ofMesh& polyvxToOfMesh, bool setColor){

    //Convienient access to the vertices and indices
	const vector<uint32_t>& vecIndices = surfaceMesh.getIndices();
	const vector<PositionMaterialNormal>& vecVertices = surfaceMesh.getVertices();//surfaceMesh.getRawVertexData();

	ofIndexType ofVecIndices;
	const void* pIndices = static_cast<const void*>(&(vecIndices[0]));

	int* indices = (int*)pIndices;

    vector<int> indx;


     for (int i = 0; i < surfaceMesh.getNoOfIndices(); i++ ){

     indx.push_back(indices[i]);
     //cout << "indices:" << indices[i] << endl;
    polyvxToOfMesh.addIndex(indx[i]);
     }

	ofLog(OF_LOG_NOTICE, "number of indices is %d", polyvxToOfMesh.getNumIndices());	

    ofVec3f ofVecVertices;


     for (int i = 0; i < surfaceMesh.getNoOfVertices(); i++ ){


    PositionMaterialNormal vert0 = vecVertices[i];


    ofVecVertices = ofVec3f(vert0.getPosition().getX(),vert0.getPosition().getY(),vert0.getPosition().getZ());

    polyvxToOfMesh.addVertex(ofVecVertices);

    }

	 ofLog(OF_LOG_NOTICE, "number of vertices is %d", polyvxToOfMesh.getNumVertices());

    ofVec3f ofVecNormals;

    for (int i = 0; i < surfaceMesh.getNoOfVertices(); i++ ){


    PositionMaterialNormal vert0 = vecVertices[i];


    ofVecNormals = ofVec3f(vert0.getNormal().getX(),vert0.getNormal().getY(),vert0.getNormal().getZ());

    polyvxToOfMesh.addNormal(ofVecNormals);

    }

	ofLog(OF_LOG_NOTICE, "number of normals is %d", polyvxToOfMesh.getNumNormals());


    if(setColor){

        for (int i = 0; i < surfaceMesh.getNoOfVertices(); i++ ){

        PositionMaterialNormal vert0 = vecVertices[i];

        uint8_t material = static_cast<uint8_t>(vert0.getMaterial() + 0.5);

        //cout << "material:" << int(material) << endl;

        ofFloatColor colour = convertMaterialIDToColour(material);

        //cout << colour << endl;

        polyvxToOfMesh.addColor(colour);

        bool col = polyvxToOfMesh.hasColors();

        //cout << "hasColors:" << col << endl;
        }

    }

}

void ofxPolyvox::polyvoxToOfVbo(const PolyVox::SurfaceMesh<PositionMaterialNormal>& surfaceMesh, ofVbo& vbo ){
     //Convienient access to the vertices and indices
	const vector<uint32_t>& vecIndices = surfaceMesh.getIndices();
	const vector<PositionMaterialNormal>& vecVertices = surfaceMesh.getVertices();


	ofIndexType ofVecIndices;
	const void* pIndices = static_cast<const void*>(&(vecIndices[0]));

	int* indices = (int*)pIndices;
	vector <int> indx;


   for (int i = 0; i < surfaceMesh.getNoOfIndices(); i++ ){

    indx.push_back(indices[i]);


    }

    vbo.setIndexData(&indx[0], indx.size(),GL_STATIC_DRAW);

	//ofVec3f ofVecVertices[surfaceMesh.getNoOfVertices()];
	ofVec3f *ofVecVertices;

    //const void* pVertices = static_cast<const void*>(&(vecVertices[0]));


    //cout << pVertices << endl;

     for (int i = 0; i < surfaceMesh.getNoOfVertices(); i++ ){


        PositionMaterialNormal vertexes = vecVertices[i];

        float x, y, z;

        x = vertexes.getPosition().getX();
        y = vertexes.getPosition().getY();
        z = vertexes.getPosition().getZ();



       ofVecVertices[i].set( x, y, z );

     }
      //cout << ofVecVertices << endl;
      cout << "ok?" << endl;
       vbo.setVertexData(&ofVecVertices[0], surfaceMesh.getNoOfVertices(), GL_STATIC_DRAW);



}

void ofxPolyvox::draw(){

    //Enable the depth buffer
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);

	glShadeModel(GL_SMOOTH);

    //Bind the index buffer
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBuffer);

	//Bind the vertex buffer
	glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
	glVertexPointer(3, GL_FLOAT, sizeof(PositionMaterialNormal), 0);
	glNormalPointer(GL_FLOAT, sizeof(PositionMaterialNormal), (GLvoid*)12);

	glDrawRangeElements(GL_TRIANGLES, m_uBeginIndex, m_uEndIndex-1, m_uEndIndex - m_uBeginIndex, GL_UNSIGNED_INT, 0);

	GLenum errCode = glGetError();
	if(errCode != GL_NO_ERROR)
	{
	  //What has replaced getErrorString() in the latest OpenGL?
	 //std::cout << "OpenGL Error: " << errCode << std::endl;
	 ofLog(OF_LOG_ERROR, "OpenGL Error: " + errCode);
	}


}

void ofxPolyvox::drawWireframe(){

    glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
    draw();

}


