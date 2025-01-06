////////////////////////////////////////////////////////////////////////////////

#include "matrix.h"
#include "OpenGLutils.h"
#include "shaderCompilation.h"

#include "simpler_ply.h"

GLFWwindow* window = nullptr;

float cube[] = {
	// Back
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, -1.0f, -1.0f,

	// Front
	-1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,

	// Bottom
	-1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, -1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,

	// Top
	-1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, -1.0f,

	// Left
	-1.0f, -1.0f, -1.0f,
	-1.0f, 1.0f, -1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, 1.0f, 1.0f,
	-1.0f, -1.0f, 1.0f,
	-1.0f, -1.0f, -1.0f,

	// Right
	1.0f, -1.0f, -1.0f,
	1.0f, 1.0f, -1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, 1.0f, 1.0f,
	1.0f, -1.0f, 1.0f,
	1.0f, -1.0f, -1.0f,
};


int main(const int argc, const char* argv[]) {

	std::string ply_path = "C:/Users/lewis/Pictures/gs/truck/point_cloud.ply";

	 //use std::unique_ptr<GSSplats> loadFromSplatsPly(const std::string& path)
	std::unique_ptr<GSSplats> splatPtr = loadFromSplatsPly(ply_path);
	if (!splatPtr->valid)
	{
		return false;
	}

	uint32_t framebufferWidth = 1280, framebufferHeight = 720;

	const int numEyes = 1;

	const int windowHeight = framebufferHeight;
	const int windowWidth = (framebufferWidth * windowHeight) / framebufferHeight;

	// INITIALIZE OPENGL
	window = initOpenGL(windowWidth, windowHeight, "Lightfield Player");

	// CAMERA INITIAL POSITION
	Vector3 bodyTranslation(0.0f, 0.0f, 0.0f);
	Vector3 bodyRotation;

	standardFrameBuffer framebuffer(framebufferWidth, framebufferHeight, numEyes); // final 

	GLuint trilinearSampler = GL_NONE;
	{
		glGenSamplers(1, &trilinearSampler);
		glSamplerParameteri(trilinearSampler, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glSamplerParameteri(trilinearSampler, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glSamplerParameteri(trilinearSampler, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glSamplerParameteri(trilinearSampler, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	double lastvideoTime = glfwGetTime();

	// Main loop:
	int timer = 0;

	GLint curDisplayFrame = 0;

	// TIME STUFF
	double lastTime = glfwGetTime();
	int nbFrames = 0;

	///////////////////////////////////////////////////////////////////////////////////////////
	// START OF PLAYER LOOP
	///////////////////////////////////////////////////////////////////////////////////////////
	float framebufferWidth_f = framebufferWidth;
	float framebufferHeight_f = framebufferHeight;

	const float nearPlaneZ = -0.1f;
	const float farPlaneZ = -200.0f;
	const float verticalFieldOfView = 45.0f * PI / 180.0f;
	const float horizontalFieldOfView = 2 * atan((framebufferWidth_f / framebufferHeight_f) * tan(verticalFieldOfView / 2));

	double previousTime = glfwGetTime();

	// get cube 
	standardShader cube_shader("basic_vrt.glsl", "basic_geo.glsl", "basic_pix.glsl");

	//const float cube_positions[][3] = {  .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f, -.5f, .5f-.5f, .5f, -.5f, -.5f, .5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, -.5f, .5f, .5f, .5f, .5f, .5f, .5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, .5f, .5f, -.5f, .5f, -.5f, -.5f, -.5f, -.5f, -.5f, -.5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, -.5f, .5f, .5f, -.5f, };
	//int num_vertices = 24;
	//int num_points = 24;

	int num_points = splatPtr->numSplats;


	float* p_positions = new float[num_points * 3];
	//float* p_normals = new float[num_points * 3];
	float* p_dc = new float[num_points * 3];
	//float* p_shs = new float[num_points * 45];
	//float* p_opacity = new float[num_points];
	float* p_scale = new float[num_points * 3];
	//float* p_rot = new float[num_points * 4];

	for (int i = 0; i < num_points; i++)
	{
		for (int j = 0; j < 3; j++) p_positions[i * 3 + j] = splatPtr->splats[i].pos[j];
		//for (int j = 0; j < 3; j++) p_normals[i * 3 + j] = splatPtr->splats[i].n[j];
		for (int j = 0; j < 3; j++) p_dc[i * 3 + j] = splatPtr->splats[i].dc[j];

		//for (int j = 0; j < 45; j++) p_shs[i * 45 + j] = splatPtr->splats[i].shs[j];

		//p_opacity[i] = splatPtr->splats[i].opacity;

		for (int j = 0; j < 3; j++) p_scale[i * 3 + j] = splatPtr->splats[i].scale[j];
		//for (int j = 0; j < 4; j++) p_rot[i * 4 + j] = splatPtr->splats[i].rot[j];
	}

	///////////////////////////////////////////////////////////////////////////////////////////
	int _numVerts = 36;

	// make the cube vao
	GLuint _vao = GL_NONE;
	glGenVertexArrays(1, &_vao);

	GLuint _vbo = GL_NONE;
	glGenBuffers(1, &_vbo);

	glBindVertexArray(_vao);
	glBindBuffer(GL_ARRAY_BUFFER, _vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(cube), cube, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 3, nullptr);
	///////////////////////////////////////////////////////////////////////////////////////////



	GLuint positionBuffer = GL_NONE;
	glGenBuffers(1, &positionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, positionBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_points * 3, p_positions, GL_STATIC_DRAW);
	GLint cube_positionAttribute = glGetAttribLocation(cube_shader.shader, "position");

	GLuint colourBuffer = GL_NONE;
	glGenBuffers(1, &colourBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, colourBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_points * 3, p_dc, GL_STATIC_DRAW);
	GLint cube_colourAttribute = glGetAttribLocation(cube_shader.shader, "dc");
	if (cube_colourAttribute == -1) {
		std::cout << "Warning: dc attribute not found in shader" << std::endl;
		// Handle error
	}
	GLuint scaleBuffer = GL_NONE;
	glGenBuffers(1, &scaleBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, scaleBuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * num_points * 3, p_scale, GL_STATIC_DRAW);
	GLint cube_scaleAttribute = glGetAttribLocation(cube_shader.shader, "scale");
	if (cube_scaleAttribute == -1) {
		std::cout << "Warning: scale attribute not found in shader" << std::endl;
		// Handle error
	}
	///////////////////////////////////////////////////////////////////////////////////////////
	while (!glfwWindowShouldClose(window)) {
		assert(glGetError() == GL_NONE);

		// GET FRAMERATE
		double currentTime = glfwGetTime();
		nbFrames++;
		if (currentTime - lastTime >= 1.0) { // If last prinf() was more than 1 sec ago
			// printf and reset timer
			printf("%f ms/frame\n", 1000.0 / double(nbFrames));
			nbFrames = 0;
			lastTime += 1.0;
			std::cout << "Current position is: " << bodyTranslation[0] << " " << bodyTranslation[1] << " " << bodyTranslation[2] << std::endl;
			std::cout << "Current rotation is: " << bodyRotation[0] << " " << bodyRotation[1] << " " << bodyRotation[2] << std::endl;
		}

		Matrix4x4 eyeToHead[numEyes], projectionMatrix[numEyes], headToBodyMatrix;
#       ifdef _VR
		getEyeTransformations(hmd, trackedDevicePose, nearPlaneZ, farPlaneZ, headToBodyMatrix.data, eyeToHead[0].data, eyeToHead[1].data, projectionMatrix[0].data, projectionMatrix[1].data);
#       else
		projectionMatrix[0] = Matrix4x4::perspective(float(framebufferWidth), float(framebufferHeight), nearPlaneZ, farPlaneZ, verticalFieldOfView);
#       endif

		// printf("float nearPlaneZ = %f, farPlaneZ = %f; int width = %d, height = %d;\n", nearPlaneZ, farPlaneZ, framebufferWidth, framebufferHeight);

		const Matrix4x4& bodyToWorldMatrix =
			Matrix4x4::translate(bodyTranslation) *
			Matrix4x4::roll(bodyRotation.z) *
			Matrix4x4::yaw(bodyRotation.y) *
			Matrix4x4::pitch(bodyRotation.x);

		const Matrix4x4& headToWorldMatrix = bodyToWorldMatrix * headToBodyMatrix;
		const Matrix4x4& objectToWorldMatrix = Matrix4x4::translate(0.0f, 0.0f, 0.0f);

		for (int eye = 0; eye < numEyes; ++eye)
		{
			////////////////////////////////////////////////////////////////////////
			// GENERATE THE CAMERA MATRICES FOR EACH EYE

			const Matrix4x4& cameraToWorldMatrix = headToWorldMatrix * eyeToHead[eye];

			const Matrix4x4& modelViewProjectionMatrix = projectionMatrix[eye] * cameraToWorldMatrix.inverse() * objectToWorldMatrix;
			const Vector4& cameraPosition = cameraToWorldMatrix.col(3);

			glEnable(GL_PROGRAM_POINT_SIZE);

			glBindFramebuffer(GL_FRAMEBUFFER, framebuffer.buffer[eye]);
			glViewport(0, 0, framebufferWidth, framebufferHeight);

			glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			// Draw a mesh
			glEnable(GL_DEPTH_TEST);
			glDisable(GL_BLEND);

			glUseProgram(cube_shader.shader);

			glBindVertexArray(_vao);


			// Other uniforms in the interface block
			{
				glBindBufferBase(GL_UNIFORM_BUFFER, cube_shader.uniformBindingPoint, cube_shader.uniformBlock);

				GLubyte* ptr = (GLubyte*)glMapBuffer(GL_UNIFORM_BUFFER, GL_WRITE_ONLY);
				memcpy(ptr + cube_shader.uniformOffset[0], objectToWorldMatrix.data, sizeof(objectToWorldMatrix));

				memcpy(ptr + cube_shader.uniformOffset[1], modelViewProjectionMatrix.data, sizeof(modelViewProjectionMatrix));
				memcpy(ptr + cube_shader.uniformOffset[2], &cameraPosition.x, sizeof(Vector3));
				glUnmapBuffer(GL_UNIFORM_BUFFER);
			}

			//glDrawArrays(GL_POINTS, 0, num_points);
			glDrawArraysInstanced(GL_TRIANGLES, 0, _numVerts, num_points);

			//glDisableVertexAttribArray(cube_positionAttribute);
			//glDisableVertexAttribArray(cube_colourAttribute);
			//glDisableVertexAttribArray(cube_scaleAttribute);


		} // for each eye

		////////////////////////////////////////////////////////////////////////
#       ifdef _VR
		// Tell the compositor to begin work immediately instead of waiting for the next WaitGetPoses() call
		vr::VRCompositor()->PostPresentHandoff();
#       endif


		// Mirror to the window
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, GL_NONE);
		glViewport(0, 0, windowWidth, windowHeight);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBlitFramebuffer(0, 0, framebufferWidth, framebufferHeight, 0, 0, windowWidth, windowHeight, GL_COLOR_BUFFER_BIT, GL_LINEAR);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, GL_NONE);

		// Display what has been drawn on the main window
		glfwSwapBuffers(window);

		// Check for events
		glfwPollEvents();

		// Handle events
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, 1);
		}

		// WASD keyboard movement


		double newTime = glfwGetTime();
		double frameTime = newTime - previousTime;
		previousTime = newTime;

		const float cameraMoveSpeedPerSecond = 1.0f;
		const float apertureSpeed = 0.05f;
		const float focusSpeed = 1.0;

		float cameraMoveSpeed = cameraMoveSpeedPerSecond * frameTime;
		float apertureStep = apertureSpeed * frameTime;
		float focusStep = focusSpeed * frameTime;

		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_W)) { bodyTranslation += Vector3(headToWorldMatrix * Vector4(0, 0, -cameraMoveSpeed, 0)); }
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_S)) { bodyTranslation += Vector3(headToWorldMatrix * Vector4(0, 0, +cameraMoveSpeed, 0)); }
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_A)) { bodyTranslation += Vector3(headToWorldMatrix * Vector4(-cameraMoveSpeed, 0, 0, 0)); }
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_D)) { bodyTranslation += Vector3(headToWorldMatrix * Vector4(+cameraMoveSpeed, 0, 0, 0)); }
		if (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_C)) { bodyTranslation.y -= cameraMoveSpeed; }
		//if ((GLFW_PRESS == glfwGetKey(window, GLFW_KEY_SPACE)) || (GLFW_PRESS == glfwGetKey(window, GLFW_KEY_Z))) { bodyTranslation.y += cameraMoveSpeed; }

		// Keep the camera above the ground
		//if (bodyTranslation.y < -2.0f) { bodyTranslation.y = -2.0f; }

		static bool inDrag = false;
		const float cameraTurnSpeed = 0.005f;
		if (GLFW_PRESS == glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
			static double startX, startY;
			double currentX, currentY;

			glfwGetCursorPos(window, &currentX, &currentY);
			if (inDrag) {
				bodyRotation.y -= float(currentX - startX) * cameraTurnSpeed;
				bodyRotation.x -= float(currentY - startY) * cameraTurnSpeed;
			}
			inDrag = true; startX = currentX; startY = currentY;
		}
		else {
			inDrag = false;
		}

		++timer;
	}
	
}