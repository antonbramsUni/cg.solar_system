#ifndef APPLICATION_SOLAR_HPP
#define APPLICATION_SOLAR_HPP

#include "application.hpp"
#include "model.hpp"
#include "structs.hpp"

// gpu representation of model
class ApplicationSolar : public Application {
	
	public:
		
		struct Planet {
			std::string name;
			int parent;
			float diameter;
			float speed;
			float orbit;
			glm::fvec3 color;
			glm::fmat4 origin;
			texture_object textureColor;
			texture_object textureNormal;
			unsigned int id;
			Planet(
				std::string _name,
			 	int _parent,
				float _diameter,
				float _speed,
				float _orbit,
				glm::fvec3 _color,
				unsigned int _id
			){
				name 	 = _name;
				parent 	 = _parent;
				diameter = _diameter;
				speed 	 = _speed;
				orbit 	 = _orbit;
				color 	 = _color;
				id       = _id;
			};
		};
		struct CameraBlock {
			glm::fmat4 viewMatrix;
			glm::fmat4 projectionMatrix;

		};
		// allocate and initialize objects
		ApplicationSolar(std::string const& resource_path);
		// free allocated objects
		~ApplicationSolar();
		// update uniform locations and values
		void uploadUniforms();
		// update projection matrix
		void updateProjection();
		// react to key input
		void keyCallback(int key, int scancode, int action, int mods);
		//handle delta mouse movement input
		void mouseCallback(double pos_x, double pos_y);
		// draw all objects
		void render() const;
		void upload_planet_transforms(Planet &p) const;
		
	protected:
		
		void initializeShaderPrograms();
		void initializeGeometry();
		void updateView();
		
		bool mouseActive = true;
		float mouseX = 0;
		float mouseY = 0;
		float zoom   = 4;
		float slide  = 0;
		
		model_object planet_object, star_object, quad_object;
		CameraBlock camera_object;
		GLuint tex_handle, rbo_handle, fbo_handle;
		GLint shading   = 0;
		GLint fxControl = 0;
		GLuint cameraUBO = 0;
};

#endif