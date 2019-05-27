#include "Mesh.h"

using namespace std;
std::unordered_map<std::string, Mesh*> Mesh::m_meshes;

void Mesh::Release()
{
	std::unordered_map<string, Mesh*>::iterator terminator;
	for (terminator = m_meshes.begin(); terminator != m_meshes.end(); terminator++) {
		if (terminator->second) {
			delete terminator->second;
			terminator->second = 0;
		}
	}

}

Mesh* Mesh::GetMesh(string meshid) {
	if (m_meshes.count(meshid) > 0) {
		return m_meshes[meshid];
	}
	else {
		RaiseException(4, 0, 0, 0);
		return NULL;
	}
}
void Mesh::LoadMesh(string meshid, string filepath, Direct3D* renderer) {
	if (m_meshes.count(meshid) > 0) {
		delete m_meshes[meshid];
		m_meshes[meshid] = 0;
	}
	m_meshes[meshid] = new Mesh(renderer, filepath);
}

Mesh::~Mesh()
{
	if (m_indexBuffer)
	{
		m_indexBuffer->Release();
		m_indexBuffer = NULL;
	}
	if (m_vertexBuffer)
	{
		m_vertexBuffer->Release();
		m_vertexBuffer = NULL;
	}
}

Mesh::Mesh() {
	m_vertexBuffer = NULL;
	m_indexBuffer = NULL;
	m_vertexCount = 0;
	m_indexCount = 0;
}

Mesh::Mesh(Direct3D* renderer, string filepath)
{
	// Parsing the OBJ format to load meshes modelled in external tools

	m_topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;

	//A face is made of three points, each one containing a position, normal and texture coord (uv)
	//This struct is used during the load to store this data
	struct Face
	{
		int vert1, uv1, normal1;
		int vert2, uv2, normal2;
		int vert3, uv3, normal3;
	};

	DirectX::XMFLOAT3* verts;		//This array stores raw positional data from the OBJ file. 
	DirectX::XMFLOAT3* normals;	//This array stores the raw normals from the OBJ file
	DirectX::XMFLOAT2* uvs;		//This array stores the raw texture coords from the OBJ file
	Face* faces;		//This is an array of Face structs that collects the different 
						//vert, normal and uv indices into a single place

	int vertexCount = 0;	//We will work out the total number of verts in the file
	int normalCount = 0;	//The total number of normals	
	int textureCount = 0;	//The total number of texture coords
	int faceCount = 0;		//The total number of faces

	int vertIndex = 0;		//These are used to keep track of where we are upto in our data arrays
	int normalIndex = 0;
	int uvIndex = 0;
	int faceIndex = 0;

	ifstream fileIn;		//OBJ files are just text files so we use a simple input file stream

							//The first read of the file is just to determine the number of verts, normals and uvs in the file
	fileIn.open(filepath);		//Open our OBJ file for reading

	if (!fileIn.good())			//If we can't open it throw an exception (al tantrum)
		RaiseException(5, 0, 0, 0);
	char input;					//We're going to read through char by char, each char we read will go in here
	while (!fileIn.eof())		//Keep reading until the end of the file
	{
		fileIn.get(input);		//Read the first char on the line

		if (input == 'v')		//If the line starts with v then we...
		{
			fileIn.get(input);	//...want to chack the next char after it...
			if (input == ' ') { vertexCount++; }	//...if it's a blank space then we have a vertex position!
			if (input == 't') { textureCount++; }	//...if it's a 't' then we have a texture coord!
			if (input == 'n') { normalCount++; }	//...if it's an 'n' then we have a normal!
		}

		if (input == 'f')		//If the first char of the line is 'f' then it's a face
		{
			fileIn.get(input);
			if (input == ' ') { faceCount++; }
		}

		while (input != '\n')	//Now that we have checked this line, we'll skip to the end of it.
		{
			fileIn.get(input);
		}
	}
	fileIn.close();		//After reading the counts we then close the file to reset it

						//Now that we know how much memory to allocate we can read the Data from the OBJ file
	fileIn.open(filepath);
	if (!fileIn.good())
		RaiseException(5, 0, 0, 0);

	//Allocate our arrys
	verts = new DirectX::XMFLOAT3[vertexCount];
	normals = new DirectX::XMFLOAT3[normalCount];
	uvs = new DirectX::XMFLOAT2[textureCount];
	faces = new Face[faceCount];

	while (!fileIn.eof())		//We will read the whole file again
	{
		fileIn.get(input);		//Char by Char
		if (input == 'v')		//If it is starts with v...
		{
			fileIn.get(input);

			if (input == ' ')	//...and is a vert position...
			{
				//...then read from the file in to the vert array at vertIndex...
				fileIn >> verts[vertIndex].x >> verts[vertIndex].y >> verts[vertIndex].z;
				vertIndex++;	//...and increase the vertIndex
			}
			if (input == 't')	//...and is a tex coord...
			{
				//...then read from the file in to the uv array at uvIndex...
				fileIn >> uvs[uvIndex].x >> uvs[uvIndex].y;
				uvs[uvIndex].y = -uvs[uvIndex].y;
				uvIndex++;	//...and increase the uvIndex
			}
			if (input == 'n')	//...and is a normal...
			{
				//...then read from the file in to the normal array at normalIndex...
				fileIn >> normals[normalIndex].x >> normals[normalIndex].y >> normals[normalIndex].z;
				normalIndex++;	//...and increase the normalIndex
			}
		}
		if (input == 'f')	//If we're reading a line describing a face then we need to fill out one of the Face structs
		{
			char junk;		//The face lines are in the format of "index/index/index" which means that the / chars are junk

							//Here you can see us reading three verts worth of information for each face. When we expect a / char we put it into "junk"
							//(NOTE: OBJ can support faces with more then 3 points, we only read for 3. This means that our models must be made up of
							//		 only triangles (i.e. they must be "triangulated"))
			fileIn >> faces[faceIndex].vert1 >> junk >> faces[faceIndex].uv1 >> junk >> faces[faceIndex].normal1
				>> faces[faceIndex].vert2 >> junk >> faces[faceIndex].uv2 >> junk >> faces[faceIndex].normal2
				>> faces[faceIndex].vert3 >> junk >> faces[faceIndex].uv3 >> junk >> faces[faceIndex].normal3;
			faceIndex++;	//After we read a face we increase the face index
		}

		while (input != '\n')	//Now that we have checked this line, we'll skip to the end of it.
		{
			fileIn.get(input);
		}
	}
	fileIn.close();		//Close the file, we are done with our file IO now

						//Each face is made up of three verts so the total number of indices is 3 * the number of faces
	int indexCount = faceCount * 3;
	//We will also allocate the same number of vertices
	int finalVertexCount = faceCount * 3;

	m_indexCount = indexCount;
	m_vertexCount = finalVertexCount;

	//(NOTE: There is a bit of an inefficiency in this loading code, the number of vertices is the same as the number of indices
	//		 this means that there is a lot of repeated vertex data. It's difficult because some position have different normals which
	//		 means you need to duplicate some vertices but not others.
	//		 This could be optimised by working out the final list of unique verts and creating an index buffer drawing them in the right order.)

	Vertex* vertexData = new Vertex[finalVertexCount];		//We'll allocate our vertex memory
	if (!vertexData)									//Big models could run out memory, we check for that
		RaiseException(5, 0, 0, 0);

	int vertexCounter = 0;
	for (int i = 0; i < faceCount; i++)					//For each face
	{
		int vertIndex = faces[i].vert1 - 1;				//Look up the vertex index,
		int normIndex = faces[i].normal1 - 1;			//normal index
		int uvIndex = faces[i].uv1 - 1;					//and uv index for the first point in the face

		vertexData[vertexCounter].position = verts[vertIndex];		//Fill out our vertex with the correct vert,
		vertexData[vertexCounter].colour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);			//colour (this was a method parameter),
		vertexData[vertexCounter].texCoord = uvs[uvIndex];			//uv,
		vertexData[vertexCounter].normal = normals[normIndex];		//and normal data
		vertexCounter++;

		vertIndex = faces[i].vert2 - 1;		//Move to the next point
		normIndex = faces[i].normal2 - 1;
		uvIndex = faces[i].uv2 - 1;

		vertexData[vertexCounter].position = verts[vertIndex];	//and read the data for that point
		vertexData[vertexCounter].colour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertexData[vertexCounter].texCoord = uvs[uvIndex];
		vertexData[vertexCounter].normal = normals[normIndex];
		vertexCounter++;

		vertIndex = faces[i].vert3 - 1;		//Move to the last point
		normIndex = faces[i].normal3 - 1;
		uvIndex = faces[i].uv3 - 1;

		vertexData[vertexCounter].position = verts[vertIndex];	//and read the last point for the face
		vertexData[vertexCounter].colour = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		vertexData[vertexCounter].texCoord = uvs[uvIndex];
		vertexData[vertexCounter].normal = normals[normIndex];
		vertexCounter++;
	}

	unsigned long* indexData = new unsigned long[indexCount];		//Allocate our index buffer
	if (!indexData)													//Big models could run out of memory, we check for that
		RaiseException(5, 0, 0, 0);

	for (int i = 0; i < indexCount; i++)		//Fill it out as just a linear array of numbers (NOTE: as mentioned above this could be optimised!)
	{
		indexData[i] = i;
	}

	if (!InitialiseBuffers(renderer, vertexData, indexData))	//Now that we have our vertex and index data, we need to copy it into buffers
	{
		RaiseException(5, 0, 0, 0);
	}


	//Calculate all of our bounding values
	float minX = 0, minY = 0, minZ = 0;
	float maxX = 0, maxY = 0, maxZ = 0;

	for (int i = 0; i < finalVertexCount; i++)
	{
		DirectX::XMFLOAT3 currentPos = vertexData[i].position;

		if (currentPos.x > maxX)
			maxX = currentPos.x;
		else if (currentPos.x < minX)
			minX = currentPos.x;

		if (currentPos.y > maxY)
			maxY = currentPos.y;
		else if (currentPos.y < minY)
			minY = currentPos.y;

		if (currentPos.z > maxZ)
			maxZ = currentPos.z;
		else if (currentPos.z < minZ)
			minZ = currentPos.z;
	}

	m_minVector = MathsHelper::GetXMVECTOR3(minX, minY, minZ);
	m_maxVector = MathsHelper::GetXMVECTOR3(maxX, maxY, maxZ);
	using namespace DirectX;
	// calculating the radius is more fun with native directX functions.
	XMStoreFloat(&m_radius, XMVector3Length(m_maxVector - m_minVector)); // Radius is half the distance between min and max
	m_radius /= 2;
	m_centre = (m_maxVector - m_minVector) / 2;
	m_top = m_centre;
	m_top.m128_f32[1] = maxY;
	m_bottom = m_centre;
	m_bottom.m128_f32[1] = minY;
	//Now that the buffers are created we can delete all of the data we loaded!
	if (verts)
	{
		delete[] verts;
		verts = NULL;
	}

	if (normals)
	{
		delete[] normals;
		normals = NULL;
	}

	if (uvs)
	{
		delete[] uvs;
		uvs = NULL;
	}

	if (faces)
	{
		delete[] faces;
		faces = NULL;
	}

	if (vertexData)
	{
		delete[] vertexData;
		vertexData = NULL;
	}

	if (indexData)
	{
		delete[] indexData;
		indexData = NULL;
	}
}


void Mesh::Render(Direct3D* renderer, DirectX::XMMATRIX world, Camera* cam, Shader* shader, Texture* texture)
{
	unsigned int stride;
	unsigned int offset;

	stride = sizeof(Vertex);
	offset = 0;
	renderer->GetDeviceContext()->IASetVertexBuffers(0, 1, &m_vertexBuffer, &stride, &offset);
	renderer->GetDeviceContext()->IASetIndexBuffer(m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);
	renderer->GetDeviceContext()->IASetPrimitiveTopology(m_topology);

	if (renderer->GetCurrentShader() != shader)
	{
		shader->Begin(renderer->GetDeviceContext());
		renderer->SetCurrentShader(shader);
	}

	//If there is a texture to use then set it in the shader
	if (texture)
	{
		shader->SetTexture(renderer->GetDeviceContext(), texture->GetShaderResourceView());
	}

	shader->SetMatrices(renderer->GetDeviceContext(), world, cam->GetView(), cam->GetProjection());

	// Once the buffers, shaders and matrices are set then we are ready to render.
	// We tell renderer how many indices we want to render
	renderer->GetDeviceContext()->DrawIndexed(m_indexCount, 0, 0);
}

bool Mesh::InitialiseBuffers(Direct3D* renderer, Vertex* vertexData, unsigned long* indexData)
{
	//Here we need to create and initialise a Direct3D buffer to hold and vertices and one to hold our indices

	D3D11_BUFFER_DESC vertexBufferDescription;		//Each buffer has information (stored in a struct) describing the buffer itself...
	D3D11_BUFFER_DESC indexBufferDescription;
	D3D11_SUBRESOURCE_DATA vertexDataDescription;	//...and information describing the data that needs to be copied into it,...
	D3D11_SUBRESOURCE_DATA indexDataDescription;	//...these structs are then passed to the Create buffer method so that Direct3D can allocate things correctly

	vertexBufferDescription.Usage = D3D11_USAGE_DEFAULT;					//The buffer description struct needs to know what the buffer will be used for...
	vertexBufferDescription.ByteWidth = sizeof(Vertex) * m_vertexCount;		//...how big the buffer should be in bytes (for us it is the size of one vertex * number of verts)...
	vertexBufferDescription.BindFlags = D3D11_BIND_VERTEX_BUFFER;			//...what the buffer is for (ours is a vertex buffer)...
	vertexBufferDescription.CPUAccessFlags = 0;								//...the rest a misc setting for various things that we won't need to worry about.
	vertexBufferDescription.MiscFlags = 0;									//If you want more info look up D3D11_BUFFER_DESC on the MSDN
	vertexBufferDescription.StructureByteStride = 0;

	vertexDataDescription.pSysMem = vertexData;		//The most important piece of information for the data description struct is a pointer 
													//to the system memory that we are coping into the buffer
	vertexDataDescription.SysMemPitch = 0;			//These values allow us to shift and offset the data but we won't be needing that!
	vertexDataDescription.SysMemSlicePitch = 0;

	indexBufferDescription.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDescription.ByteWidth = sizeof(unsigned long) * m_indexCount;
	indexBufferDescription.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDescription.CPUAccessFlags = 0;
	indexBufferDescription.MiscFlags = 0;
	indexBufferDescription.StructureByteStride = 0;

	indexDataDescription.pSysMem = indexData;
	indexDataDescription.SysMemPitch = 0;
	indexDataDescription.SysMemSlicePitch = 0;

	//After we have the buffer and data descriptions complete we ask our device to create the buffer for us. If successful the buffer will be availabe through m_vertexBuffer!
	if (FAILED(renderer->GetDevice()->CreateBuffer(&vertexBufferDescription, &vertexDataDescription, &m_vertexBuffer)))
	{
		return false;
	}
	//Creating the index buffer is pretty much the same as the vertex buffer
	if (FAILED(renderer->GetDevice()->CreateBuffer(&indexBufferDescription, &indexDataDescription, &m_indexBuffer)))
	{
		return false;
	}

	return true;
}