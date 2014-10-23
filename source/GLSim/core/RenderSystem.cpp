#include "GLSim/core/RenderSystem.h"
#include "GLSim/core/Transform.h"

RenderSystem::RenderSystem()
	:m_shaderManager()
{
	if(!m_shaderManager.addShader(ShaderManager::SHADER_TYPE::VERTEX, "C:/Projects/3DSim/res/shaders/BasicVertex.vs", 0))
		fprintf(stdout, "Error adding Vertex Shader\n");
	if(!m_shaderManager.addShader(ShaderManager::SHADER_TYPE::FRAGMENT, "C:/Projects/3DSim/res/shaders/BasicFragment.fs", 0))
		fprintf(stdout, "Error adding Fragment Shader\n");

	m_shaderManager.linkProgram(0);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
}

RenderSystem::~RenderSystem()
{
	
}

void RenderSystem::setCamera(Object* camera)
{
	p_camera = camera;
	glm::vec3 color = p_camera->getComponent<Camera>()->getClearColor();
	glClearColor(color.x, color.y, color.z, 1.0);
}

bool RenderSystem::addLight(Object* l)
{
	if(l->getComponent<Light>() != nullptr)
	{
		switch(l->getComponent<Light>()->getLightType())
		{
		case Light::LIGHT_TYPES::DIRECTIONAL:
			m_dirLightList.push_back(l);
		default:
			break;
		}
		return true;
	}
	return false;

}

void RenderSystem::render(float interp, Window* window)
{
	if(p_camera)
	{
		glClear(GL_COLOR_BUFFER_BIT);
		m_shaderManager.useProgram(0);

		/*glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

		glDrawArrays(GL_TRIANGLES, 0, 12*3);
		glDisableVertexAttribArray(0);*/
		p_camera->getComponent<Transform>()->receiveMessage(MessageRender(interp, &m_shaderManager, glm::mat4(1.0), false));
		p_camera->getComponent<Camera>()->receiveMessage(MessageRender(interp, &m_shaderManager, glm::mat4(1.0), false));
		//Transform* t = p_camera->getComponent<Transform>().get();
		//glm::mat4 view = glm::lookAt(t->getPosition(), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		Transform* cam = p_camera->getComponent<Transform>();
		//glm::mat4 view = cam->getTranslationMat()._inverse() * cam->getRotationMat();
		glm::mat4 view = glm::inverse(cam->getTranslationMat() * cam->getRotationMat());//glm::lookAt(cam->getPosition(), cam->getForward(), cam->getUp());
		//glm::mat4 v2 = glm::lookAt(glm::vec3(0, 0, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
		/*sendMessage(MessageRender(interp, &m_shaderManager, p_camera->getComponent<Camera>()->getPerspective() *
			p_camera->getComponent<Transform>()->getModelNoScale(), true));*/


		for(unsigned int i = 0; i < m_dirLightList.size(); ++i)
		{
			m_shaderManager.setUniform("dirLights[" + std::to_string(i) + "].light.color", glm::vec3(1, 0, 0));//m_dirLightList[i]->getComponent<Light>()->getColor());
			m_shaderManager.setUniform("dirLights[" + std::to_string(i) + "].light.intensity", m_dirLightList[i]->getComponent<Light>()->getIntensity());
			m_shaderManager.setUniform("dirLights[" + std::to_string(i) + "].direction", glm::vec3(0, 4, 0));//m_dirLightList[i]->getComponent<Transform>()->getOrientation() * glm::vec3(0, -1, 0));
		}
		m_shaderManager.setUniform("numDirLights", int(m_dirLightList.size()));
		sendMessage(MessageRender(interp, &m_shaderManager, p_camera->getComponent<Camera>()->getPerspective() * view, true));
	}

	m_shaderManager.unbindProgram();
	glfwSwapBuffers(window->getWindow());
}
