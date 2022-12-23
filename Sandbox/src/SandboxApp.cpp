#include "SandboxApp.h"

#include "imgui/imgui.h"

#include "GLFW/include/GLFW/glfw3.h"

Hazel::Application* Hazel::CreateApplication()
{ 
	return new Sandbox();
}

Sandbox::Sandbox()
{
	PushLayer(new ExampleLayer());
}
Sandbox::~Sandbox()
{
}

ExampleLayer::ExampleLayer()
	: Layer("SandboxApp"), m_OrthoCamera(-1.6f, 1.6f, -0.9f, 0.9f), m_PerspectiveCamera(70.0f, 1.777777f, 0.01f, 100.0f),
	m_CameraPositionOrtho(0, 0, 0), m_CameraPositionPerspective(0, 0, 1), 
	m_SquarePosition(0, 0, 0)
{
	
#pragma region triangle

	float vertices[3 * 7] = {
		-0.5f, -0.5f, 0.0f,		0.0f, 0.0f, 1.0f, 1.0f,
		 0.5f, -0.5f, 0.0f,		1.0f, 0.0f, 0.0f, 1.0f,
		 0.0f,  0.5f, 0.0f,		0.0f, 1.0f, 0.0f, 1.0f
	};

	m_VertexArray.reset(Hazel::VertexArray::Create());
	std::shared_ptr<Hazel::VertexBuffer> vertexBuffer;
	vertexBuffer.reset(Hazel::VertexBuffer::Create(vertices, sizeof(vertices)));
	Hazel::BufferLayout layout = {
		{ Hazel::ShaderDataType::Float3, "a_Position" },
		{ Hazel::ShaderDataType::Float4, "a_Color" }
	};
	vertexBuffer->SetLayout(layout);
	m_VertexArray->AddVertexBuffer(vertexBuffer);

	unsigned int indices[3] = { 0, 1, 2 };
	std::shared_ptr<Hazel::IndexBuffer> indexBuffer;
	indexBuffer.reset(Hazel::IndexBuffer::Create(indices, sizeof(indices) / sizeof(unsigned int)));
	m_VertexArray->SetIndexBuffer(indexBuffer);

#pragma endregion

#pragma region square

	m_SquareVertexArray.reset(Hazel::VertexArray::Create());
	float sqaureVertices[3 * 4] = {
		 -0.5f, -0.5f, 0.0f,
		  0.5f, -0.5f, 0.0f,
		  0.5f,  0.5f, 0.0f,
		 -0.5f,  0.5f, 0.0f
	};
	std::shared_ptr<Hazel::VertexBuffer> squareVertexBuffer;
	squareVertexBuffer.reset(Hazel::VertexBuffer::Create(sqaureVertices, sizeof(sqaureVertices)));
	squareVertexBuffer->SetLayout({
		{ Hazel::ShaderDataType::Float3, "a_Position" }
		});
	m_SquareVertexArray->AddVertexBuffer(squareVertexBuffer);

	unsigned int sqaureIndices[6] = { 0, 1, 2, 2, 3, 0 };
	std::shared_ptr<Hazel::IndexBuffer> squareIndexBuffer;
	squareIndexBuffer.reset(Hazel::IndexBuffer::Create(sqaureIndices, sizeof(sqaureIndices) / sizeof(unsigned int)));
	m_SquareVertexArray->SetIndexBuffer(squareIndexBuffer);

#pragma endregion

#pragma region triangle Source

	std::string vertexSource = R"(
			#version 330 core
			
			layout (location = 0) in vec3 a_Position;
			layout (location = 1) in vec4 a_Color;

			out vec3 v_Position;
			out vec4 v_Color;
			
			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main() {
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1);
			}
		)";
	std::string fragmentSource = R"(
			#version 330 core
			
			layout (location = 0) out vec4 o_Color;

			in vec3 v_Position;
			in vec4 v_Color;
			
			void main() {
				//color = vec4(v_Position * 0.5 + 0.5, 1.0);
				o_Color = v_Color;
			}
		)";

	m_Shader.reset(new Hazel::Shader(vertexSource, fragmentSource));

#pragma endregion

#pragma region square Source

	std::string sqaureVertexSource = R"(
			#version 330 core
			
			layout (location = 0) in vec3 a_Position;

			out vec3 v_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			void main() {
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform* vec4(a_Position, 1);
			}
		)";
	std::string sqaureFragmentSource = R"(
			#version 330 core
			
			layout (location = 0) out vec4 o_Color;

			in vec3 v_Position;
			
			void main() {
				o_Color = vec4(0.2, 0.3, 1.0, 1.0); 
			}

		)";

	m_SquareShader.reset(new Hazel::Shader(sqaureVertexSource, sqaureFragmentSource));

#pragma endregion

}

void ExampleLayer::OnUpdate(Hazel::Timestep ts)
{
	//HZ_TRACE("Delta Time: {0}s,	({1}ms),	fps:{2}", ts.GetSeconds(), ts.GetMilliseconds(), ts.GetMilliseconds()*5000);


	if (Hazel::Input::IsKeyPressed(HZ_KEY_LEFT))
		m_CameraPositionOrtho.x -= m_CameraSpeed * ts;
	else if (Hazel::Input::IsKeyPressed(HZ_KEY_RIGHT))
		m_CameraPositionOrtho.x += m_CameraSpeed * ts;

	if (Hazel::Input::IsKeyPressed(HZ_KEY_DOWN))
		m_CameraPositionOrtho.y -= m_CameraSpeed * ts;
	else if (Hazel::Input::IsKeyPressed(HZ_KEY_UP))
		m_CameraPositionOrtho.y += m_CameraSpeed * ts;

	if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
		m_CameraPositionPerspective.x -= m_CameraSpeed * ts;
	else if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
		m_CameraPositionPerspective.x += m_CameraSpeed * ts;

	if (Hazel::Input::IsKeyPressed(HZ_KEY_S))
		m_CameraPositionPerspective.y -= m_CameraSpeed * ts;
	else if (Hazel::Input::IsKeyPressed(HZ_KEY_W))
		m_CameraPositionPerspective.y += m_CameraSpeed * ts;


	if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_LEFT))
		m_CameraPositionPerspective.z += m_CameraSpeed * ts;
	if (Hazel::Input::IsMouseButtonPressed(HZ_MOUSE_BUTTON_RIGHT))
		m_CameraPositionPerspective.z -= m_CameraSpeed * ts;

	/*	if (Hazel::Input::IsKeyPressed(HZ_KEY_A))
			m_CameraRotation += m_CameraRotationSpeed * ts;
		else if (Hazel::Input::IsKeyPressed(HZ_KEY_D))
			m_CameraRotation -= m_CameraRotationSpeed * ts;			*/


	Hazel::RenderCommand::SetClearColor({ m_BGColor[0], m_BGColor[1], m_BGColor[2], m_BGColor[3] });
	Hazel::RenderCommand::Clear();

	m_PerspectiveCamera.SetPosition(m_CameraPositionPerspective);
	m_PerspectiveCamera.SetRotation(m_CameraRotation);

	Hazel::Renderer::BeginScene(m_PerspectiveCamera);

	glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

	for (int y = 0; y < 20; y++)
	{
		for (int x = 0; x < 20; x++)
		{
			glm::vec3 position(x * 0.11f, y * 0.11f, 0);

			glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * scale;

			Hazel::Renderer::Submit(m_SquareShader, m_SquareVertexArray, transform);
		}
	}
	Hazel::Renderer::Submit(m_Shader, m_VertexArray);
	
	Hazel::Renderer::EndScene();
}

void ExampleLayer::OnImGuiRender()
{
	ImGui::Begin("Sandbox app");
	ImGui::ColorEdit4("color", m_BGColor);
	ImGui::End();
}

void ExampleLayer::OnEvent(Hazel::Event& e)
{
	Hazel::EventDispatcher dispatcher(e);
	dispatcher.Dispatch<Hazel::KeyPressedEvent>(HZ_BIND_EVENT_FN(ExampleLayer::OnKeyPressedEvent));
}

bool ExampleLayer::OnKeyPressedEvent(Hazel::KeyPressedEvent& event)
{
	if (event.GetKeyCode() == HZ_KEY_ESCAPE) // if key is escape
	{
		Hazel::Application& app = Hazel::Application::Get(); // get application
		app.StopRunning(); // stop app
	}

	return true; // we handled it
}
