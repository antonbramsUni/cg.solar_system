// https://www.uni-weimar.de/fileadmin/user/fak/medien/professuren/Computer_Graphics/course_material/CG_Exercise_1718/CG_assignments.pdf

#include "application_solar.hpp"
#include "launcher.hpp"
#include "utils.hpp"
#include "shader_loader.hpp"
#include "model_loader.hpp"
#include "texture_loader.hpp"
#include <glbinding/gl/gl.h>
#include <stdlib.h>
// use gl definitions from glbinding 
using namespace gl;
// dont load gl bindings from glfw
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

std::vector <ApplicationSolar::Planet> planets = {
	// values modified to appear in the screen.
	// http://nssdc.gsfc.nasa.gov/planetary/factsheet/planet_table_ratio.html
	// The sun and moon were modified by a n/10 size
	{"sun",     -1, 10.93800f,    0.000f,  0.00000f, {1,1,0}, 0},
	{"mercury",  0,   0.3830f,   58.800f,  0.38700f, {1,0,1}, 1},
	{"venus",    0,   0.9490f, -244.000f,  0.72300f, {1,1,0}, 2},
	{"earth",    0,   1.0000f,    1.000f,  1.00000f, {0,1,1}, 3},
	{"moon",     3,   0.2724f,   27.400f,  0.02560f, {1,0,1}, 4},
	{"mars",     0,   0.5320f,    1.030f,  1.52000f, {1,1,0}, 5},
	{"jupiter",  0,  11.2100f,    0.415f,  5.20000f, {0,1,1}, 6},
	{"saturn",   0,   9.4500f,    0.445f,  9.58000f, {1,0,1}, 7},
	{"uranus",   0,   4.0100f,   -0.720f, 19.20000f, {1,1,0}, 8},
	{"neptune",  0,   3.8800f,    0.673f, 30.05000f, {0,1,1}, 9}
};

texture_object star_texture;

// random function that generates position or colors
float random (float min, float max) {
	return (max - min) * (float(rand() % 100) / 100) + min;
}

ApplicationSolar::ApplicationSolar(std::string const& resource_path):
	Application {resource_path}, planet_object {}, star_object {} {
		std::string textures = resource_path + "textures/";
		// init stars
		star_texture = utils::create_texture_object(
			texture_loader::file(textures +"stars.png"));
		// generate texture for every planet
		for (std::vector<Planet>::iterator p = planets.begin(); 
			p != planets.end(); ++ p) {
				std::string path = textures + p->name +".png";
				std::cout << "[loading color] "+ path<< std::endl;
				p->textureColor = utils::create_texture_object(
					texture_loader::file(path));
				if (p->name == "earth") {
					std::string path = textures + p->name +"_normal.png";
					std::cout << "[loading normal] "+ path << std::endl;
					p->textureNormal = utils::create_texture_object(
						texture_loader::file(path));
				}
			}
		// initialize other stuff
		initializeGeometry();
		initializeShaderPrograms();
	}

void ApplicationSolar::upload_planet_transforms(Planet &p) const {
	// bind shader to upload uniforms
	glUseProgram(m_shaders.at("planet").handle);
	// bind texture_color
	glActiveTexture(GL_TEXTURE0 + p.id + 1);
	glBindTexture(
		GL_TEXTURE_2D, 
		p.textureColor.handle);
	glUniform1i(
		m_shaders.at("planet").u_locs.at("TextureColor"), 
		p.id + 1);
	// bind texture_normal
	glActiveTexture(GL_TEXTURE0 + p.id + 1 + 10);
	glBindTexture(
		GL_TEXTURE_2D, 
		p.textureNormal.handle);
	glUniform1i(
		m_shaders.at("planet").u_locs.at("TextureNormal"), 
		p.id + 1 + 10);
	// create origin matrix accordint to a model
	// if p.parent is -1 then set the origin to the world matrix
	// otherwise find a parent matrix in the model
	p.origin = p.parent == -1? glm::fmat4 {}: 
		planets.at((unsigned long) p.parent).origin;
	// orbit and rotation
	if (p.orbit != 0.0f) {
		float angle = glfwGetTime() * p.speed * 0.01f;
		// rotate around an origin
		p.origin = glm::rotate(
		  	p.origin, angle, glm::fvec3{0, 1.0f, 0});
		// an orbit distance
		p.origin = glm::translate(
		  	p.origin, glm::fvec3 {p.orbit * 0.8f, 0, 0});
	}
	// scale the planet to it's size
	glm::fmat4 model_matrix = glm::scale(
		p.origin, glm::vec3(p.diameter / 100)); // 100
	// do open gl routine 
	// assigning 2 to the sun for shading in the if statement 
	GLint id = p.id;
	glUniform1iv(
		m_shaders.at("planet").u_locs.at("ID"),
		1, &id);
	glUniform1iv(
		m_shaders.at("planet").u_locs.at("Shading"),
		1, &shading);
	glUniformMatrix4fv(
		m_shaders.at("planet").u_locs.at("ModelMatrix"),
		1, GL_FALSE, glm::value_ptr(model_matrix));
	// bind the VAO to draw
	glBindVertexArray(planet_object.vertex_AO);
	// draw bound vertex array using bound shader
	glDrawElements(
		planet_object.draw_mode, 
		planet_object.num_elements, 
		model::INDEX.type, 
		NULL);
}

void ApplicationSolar::render() const {
	// draw planets
	for (std::vector<Planet>::iterator p = planets.begin(); 
		p != planets.end(); ++ p)
			upload_planet_transforms(*p);
	// bind star shaders
	glUseProgram(m_shaders.at("star").handle);
	// bind texture
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(
		GL_TEXTURE_2D, 
		star_texture.handle);
	glUniform1i(
		m_shaders.at("star").u_locs.at("ColorTex"), 0);
	// do transformations
	glm::fmat4 model_matrix = glm::scale(
		glm::fmat4 {}, glm::vec3(10));
	glUniformMatrix4fv(
		m_shaders.at("star").u_locs.at("ModelMatrix"),
		1, GL_FALSE, glm::value_ptr(model_matrix));
	// bind the VAO to draw
	glBindVertexArray(star_object.vertex_AO);
	// draw bound vertex array using bound shader
	glDrawElements(
		star_object.draw_mode, 
		star_object.num_elements, 
		model::INDEX.type, 
		NULL);
}

void ApplicationSolar::updateView() {
	// reset the view transform in order to manipulate it
	m_view_transform = glm::fmat4 {};
	m_view_transform = glm::rotate(
		m_view_transform, -mouseX, glm::fvec3{0, 1.0f, 0});
	m_view_transform = glm::rotate(
		m_view_transform, -mouseY, glm::fvec3{1.0f, 0, 0});
	m_view_transform = glm::translate(
		m_view_transform, glm::fvec3{slide, 0, zoom});
	// vertices are transformed in camera space, so camera transform must be inverted
	glm::fmat4 view_matrix = glm::inverse(m_view_transform);
	glUseProgram(m_shaders.at("planet").handle);
	// upload matrix to gpu
	glUniformMatrix4fv(
		m_shaders.at("planet").u_locs.at("ViewMatrix"),
		1, GL_FALSE, glm::value_ptr(view_matrix));
	glUniform3fv(
		m_shaders.at("planet").u_locs.at("LightOrigin"),
		1, glm::value_ptr(glm::fvec3{0,0,0}));
	// upload matrix to gpu
	glUseProgram(m_shaders.at("star").handle);
	glUniformMatrix4fv(
		m_shaders.at("star").u_locs.at("ViewMatrix"),
		1, GL_FALSE, glm::value_ptr(view_matrix));
}

void ApplicationSolar::updateProjection() {
	// upload planet matrix to gpu
	glUseProgram(m_shaders.at("planet").handle);
	glUniformMatrix4fv(
		m_shaders.at("planet").u_locs.at("ProjectionMatrix"),
		1, GL_FALSE, glm::value_ptr(m_view_projection));
	// upload star matrix to gpu
	glUseProgram(m_shaders.at("star").handle);
	glUniformMatrix4fv(
		m_shaders.at("star").u_locs.at("ProjectionMatrix"),
		1, GL_FALSE, glm::value_ptr(m_view_projection));
}

// update uniform locations
void ApplicationSolar::uploadUniforms() {
	updateUniformLocations();
	// planet
	updateView();
	updateProjection();
}

// handle key input
void ApplicationSolar::keyCallback(int key, int scancode, int action, int mods) {
	// Key can be continued pressed to move around the screen.
	// on every keystroke press / hold the 
	// zoom or slide parameter will be modified and 
	// the view matrix will be updated
	// std::cout << key << std::endl;
	float speed = .1f;
	// KEY w
	if (key == GLFW_KEY_W && action != GLFW_RELEASE) {
		zoom -= speed;
		updateView();
	// KEY s
	} else if (key == GLFW_KEY_S && action != GLFW_RELEASE) {
		zoom += speed;
		updateView();
	// key a
	} else if (key == GLFW_KEY_A && action != GLFW_RELEASE) {
		slide -= speed;
		updateView();
	// key d
	} else if (key == GLFW_KEY_D && action != GLFW_RELEASE) {
		slide += speed;
		updateView();
	}
	// Phong vs Cell Shading
	shading = key == 49? 0: key == 50? 1: shading;
}

// handle delta mouse movement input
void ApplicationSolar::mouseCallback(double pos_x, double pos_y) {
	// this method activate the mouse and can be used to move around the solar system
	// in case if we have a mouse down event
	if (mouseActive) { 
		float speed = .01f;
		mouseX += pos_x * speed;
		mouseY += pos_y * speed;
		updateView();
	}
}

// load shader programs
void ApplicationSolar::initializeShaderPrograms() {
	// store shader program objects in container
	m_shaders.emplace("planet", 
		shader_program{m_resource_path + "shaders/simple.vert",
		m_resource_path + "shaders/simple.frag"});
	// request uniform locations for planet shader program
	m_shaders.at("planet").u_locs["ModelMatrix"]      = -1;
	m_shaders.at("planet").u_locs["ViewMatrix"]       = -1;
	m_shaders.at("planet").u_locs["ProjectionMatrix"] = -1;
	m_shaders.at("planet").u_locs["LightOrigin"]      = -1;
	m_shaders.at("planet").u_locs["Shading"]      	  = -1;
	m_shaders.at("planet").u_locs["TextureColor"] 	  = -1;
	m_shaders.at("planet").u_locs["TextureNormal"] 	  = -1;
	m_shaders.at("planet").u_locs["ID"] 		  	  = -1;
	// store shader program objects in container
	m_shaders.emplace("star", 
		shader_program{m_resource_path + "shaders/star.vert",
		m_resource_path + "shaders/star.frag"});
	// request uniform locations for star shader program
	m_shaders.at("star").u_locs["ModelMatrix"]      = -1;
	m_shaders.at("star").u_locs["ViewMatrix"]       = -1;
	m_shaders.at("star").u_locs["ProjectionMatrix"] = -1;
	m_shaders.at("star").u_locs["ColorTex"]      	= -1;
}

// load models
void ApplicationSolar::initializeGeometry() {
	// load a planet object and save it into a model
	model planet_model = model_loader::obj(
		m_resource_path + "models/sphere.obj", 
		model::NORMAL | model::TEXCOORD);
	// generate vertex array object
	glGenVertexArrays(1, &planet_object.vertex_AO);
	// bind the array for attaching buffers
	glBindVertexArray(planet_object.vertex_AO);
	// generate generic buffer
	glGenBuffers(1, &planet_object.vertex_BO);
	// bind this as an vertex array buffer containing all attributes
	glBindBuffer(GL_ARRAY_BUFFER, planet_object.vertex_BO);
	// configure currently bound array buffer
	glBufferData(
		GL_ARRAY_BUFFER, 
		sizeof(float) * planet_model.data.size(), 
		planet_model.data.data(), 
		GL_STATIC_DRAW);
	// activate first attribute on gpu
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 
		model::POSITION.components, 
		model::POSITION.type, GL_FALSE, 
		planet_model.vertex_bytes, 
		planet_model.offsets[model::POSITION]);
	// activate second attribute on gpu
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 
		model::NORMAL.components, 
		model::NORMAL.type, GL_FALSE, 
		planet_model.vertex_bytes, 
		planet_model.offsets[model::NORMAL]);
	// activate third attribute for colors
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 
		model::TEXCOORD.components, 
		model::TEXCOORD.type, GL_FALSE, 
		planet_model.vertex_bytes, 
		planet_model.offsets[model::TEXCOORD]);
	// generate generic buffer
	glGenBuffers(1, &planet_object.element_BO);
	// bind this as an vertex array buffer containing all attributes
	glBindBuffer(
		GL_ELEMENT_ARRAY_BUFFER, 
		planet_object.element_BO);
	// configure currently bound array buffer
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, 
		model::INDEX.size * planet_model.indices.size(), 
		planet_model.indices.data(), 
		GL_STATIC_DRAW);
	// store type of primitive to draw
	planet_object.draw_mode = GL_TRIANGLES;
	// transfer number of indices to model object 
	planet_object.num_elements = GLsizei(planet_model.indices.size());
		
	// load a skystuff object and save it into a model
	model star_model = model_loader::obj(
		m_resource_path + "models/sphere.obj", 
		model::TEXCOORD);
	// generate vertex array object
	glGenVertexArrays(1, &star_object.vertex_AO);
	// bind the array for attaching buffers
	glBindVertexArray(star_object.vertex_AO);
	// generate generic buffer
	glGenBuffers(1, &star_object.vertex_BO);
	// bind this as an vertex array buffer containing all attributes
	glBindBuffer(GL_ARRAY_BUFFER, star_object.vertex_BO);
	// configure currently bound array buffer
	glBufferData(
		GL_ARRAY_BUFFER, 
		sizeof(float) * star_model.data.size(), 
		star_model.data.data(), 
		GL_STATIC_DRAW);
	// activate first attribute on gpu
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 
		model::POSITION.components, 
		model::POSITION.type, 
		GL_FALSE, 
		star_model.vertex_bytes, 
		star_model.offsets[model::POSITION]);
	// activate second attribute on gpu
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 
		model::TEXCOORD.components,
		model::TEXCOORD.type, 
		GL_FALSE, 
		star_model.vertex_bytes, 
		star_model.offsets[model::TEXCOORD]);
	// generate generic buffer
	glGenBuffers(1, &star_object.element_BO);
	// bind this as an vertex array buffer containing all attributes
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, star_object.element_BO);
	// configure currently bound array buffer
	glBufferData(
		GL_ELEMENT_ARRAY_BUFFER, 
		model::INDEX.size * star_model.indices.size(), 
		star_model.indices.data(), 
		GL_STATIC_DRAW);
	// store type of primitive to draw
	star_object.draw_mode = GL_TRIANGLES;
	// transfer number of indices to model object 
	star_object.num_elements = GLsizei(star_model.indices.size());
}

ApplicationSolar::~ApplicationSolar() {
	// clear planet buffers
	glDeleteBuffers(1, &planet_object.vertex_BO);
	glDeleteBuffers(1, &planet_object.element_BO);
	glDeleteVertexArrays(1, &planet_object.vertex_AO);
	// clear star buffers
	glDeleteBuffers(1, &star_object.vertex_BO);
	glDeleteBuffers(1, &star_object.element_BO);
	glDeleteVertexArrays(1, &star_object.vertex_AO);
}

// exe entry point
int main(int argc, char* argv[]) {
	Launcher::run<ApplicationSolar>(argc, argv);
}
