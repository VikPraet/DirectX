#include "pch.h"
#include "Camera.h"

Camera::Camera(const Vector3& origin, float fovAngle, float aspectRatio, Vector3 target):
	m_Origin{ origin },
	m_Target{ target },
	m_FovAngle{ fovAngle },
	m_AspectRatio{ aspectRatio }
{
	m_FovAngle = fovAngle;
	m_FovValue = tanf((m_FovAngle * TO_RADIANS) / 2.f);
	m_AspectRatio = aspectRatio;

	m_Origin = origin;
	m_Target = target;
	m_StarTarget = target;
	targetStopDistance = Vector3::Distance(origin, target);

	CalculateViewMatrix();
	CalculateProjectionMatrix();
}

void Camera::CalculateViewMatrix()
{
	//ONB => invViewMatrix
	//Inverse(ONB) => ViewMatrix

	m_Right = Vector3::Cross(Vector3::UnitY, m_Forward);
	m_Right.Normalize();

	m_Up = Vector3::Cross(m_Forward, m_Right);
	m_Up.Normalize();

	m_ViewMatrix = {
		{ m_Right.x	, m_Right.y	, m_Right.z	, 0},
		{ m_Up.x		, m_Up.y		, m_Up.z		, 0},
		{ m_Forward.x	, m_Forward.y	, m_Forward.z	, 0},
		{ m_Origin.x	, m_Origin.y	, m_Origin.z	, 1}
	};

	m_InvViewMatrix = Matrix::Inverse(m_ViewMatrix);

	//ViewMatrix => Matrix::CreateLookAtLH(...) [not implemented yet]
	//DirectX Implementation => https://learn.microsoft.com/en-us/windows/win32/direct3d9/d3dxmatrixlookatlh
}

void Camera::CalculateProjectionMatrix()
{
	Vector4 x{ 1 / (m_AspectRatio * m_FovValue), 0, 0, 0 };
	Vector4 y{ 0, 1 / m_FovValue, 0, 0 };
	Vector4 z{ 0, 0, m_FarPlane / (m_FarPlane - m_NearPlane), 1 };
	Vector4 t{ 0, 0, -(m_FarPlane * m_NearPlane) / (m_FarPlane - m_NearPlane), 0 };

	m_ProjectionMatrix = { x,y,z,t };
}

void Camera::CalculateViewMatrixTargetRotation()
{
	m_Right = Vector3::Cross(Vector3::UnitY, m_Forward);
	m_Right.Normalize();

	m_Up = Vector3::Cross(m_Forward, m_Right);
	m_Up.Normalize();

	m_RotationOrigin = m_Target - m_Forward * Vector3::Distance(m_Target, m_Origin);

	m_ViewMatrix = {
		{ m_Right.x	, m_Right.y	, m_Right.z	, 0},
		{ m_Up.x		, m_Up.y		, m_Up.z		, 0},
		{ m_Forward.x	, m_Forward.y	, m_Forward.z	, 0},
		{ m_RotationOrigin.x	, m_RotationOrigin.y	, m_RotationOrigin.z	, 1}
	};

	m_InvViewMatrix = Matrix::Inverse(m_ViewMatrix);
}

void Camera::Update(const Timer* pTimer)
{
	const float deltaTime = pTimer->GetElapsed();

	//Camera Update Logic

	//Keyboard Input
	const uint8_t* pKeyboardState = SDL_GetKeyboardState(nullptr);
	bool calculateCamMatrix{};
	bool calculateProjectionMatrix{};
	// movement
	constexpr float SPEED{ 30 };
	if (pKeyboardState[SDL_SCANCODE_W])
	{
		m_Origin += m_Forward * SPEED * deltaTime;
		calculateCamMatrix = true;
	}
	if (pKeyboardState[SDL_SCANCODE_S])
	{
		m_Origin -= m_Forward * SPEED * deltaTime;
		calculateCamMatrix = true;
	}
	if (pKeyboardState[SDL_SCANCODE_A])
	{
		m_Origin -= m_Right * SPEED * deltaTime;
		calculateCamMatrix = true;
	}
	if (pKeyboardState[SDL_SCANCODE_D])
	{
		m_Origin += m_Right * SPEED * deltaTime;
		calculateCamMatrix = true;
	}
	if (pKeyboardState[SDL_SCANCODE_E])
	{
		m_Origin.y += m_Up.y * SPEED * deltaTime;
		calculateCamMatrix = true;
	}
	if (pKeyboardState[SDL_SCANCODE_Q])
	{
		m_Origin.y -= m_Up.y * SPEED * deltaTime;
		calculateCamMatrix = true;
	}


	// refocus camera to target
	static bool isRefocusing{};
	if (pKeyboardState[SDL_SCANCODE_F])
	{
		m_Target = m_StarTarget;
		isRefocusing = true;
	}

	//Smoothly move towards the target using linear interpolation
	if (isRefocusing)
	{
		constexpr float translationSpeed = 2.0f; // Adjust this speed as needed
		m_Origin = Vector3::Lerp(m_Origin, m_Target, translationSpeed * deltaTime);

		constexpr float rotationSpeed = 5.0f; // Adjust this speed as needed
		m_Forward = Vector3::Lerp(m_Forward, (m_Target - m_Origin).Normalized(), rotationSpeed * deltaTime);

		CalculateViewMatrix();
		CalculateProjectionMatrix();
	}
	if (isRefocusing && Vector3::Distance(m_Origin, m_Target) < targetStopDistance) // checks if distance to target is smaller than distance to stop from taget
	{
		m_Forward = (m_Target - m_Origin).Normalized();
		m_RotationOrigin = m_Origin;
		m_Target = m_Origin + m_Forward * Vector3::Distance(m_Target, m_Origin);

		m_TotalPitch = asinf(m_Forward.y); // Calculate pitch angle
		m_TotalYaw = atan2f(m_Forward.x, m_Forward.z); // Calculate yaw angle

		isRefocusing = false; // Refocusing is complete
	}


	// fov
	constexpr float FOV_INCREMENT{ 20 };
	constexpr int MAX_FOV{ 160 };
	constexpr int MIN_FOV{ 10 };

	if (pKeyboardState[SDL_SCANCODE_DOWN] && m_FovAngle < MAX_FOV)
	{
		m_FovAngle += FOV_INCREMENT * deltaTime;
		m_FovValue = tanf(m_FovAngle * TO_RADIANS / 2.f);
		calculateProjectionMatrix = true;
	}
	if (pKeyboardState[SDL_SCANCODE_UP] && m_FovAngle > MIN_FOV)
	{
		m_FovAngle -= FOV_INCREMENT * deltaTime;
		m_FovValue = tanf(m_FovAngle * TO_RADIANS / 2.f);
		calculateProjectionMatrix = true;
	}

	if (m_FovAngle > MAX_FOV)
	{
		m_FovAngle = MAX_FOV;
		m_FovValue = tanf(m_FovAngle * TO_RADIANS / 2.f);
	}
	if (m_FovAngle < MIN_FOV)
	{
		m_FovAngle = MIN_FOV;
		m_FovValue = tanf(m_FovAngle * TO_RADIANS / 2.f);
	}

	//Mouse Input
	int mouseX{}, mouseY{};
	const uint32_t mouseState = SDL_GetRelativeMouseState(&mouseX, &mouseY);
	constexpr float SENSITIVITY{ 0.007f };
	constexpr float MOVEMENT_SENSITIVITY{ 0.08f };

	if (mouseState & SDL_BUTTON_RMASK && pKeyboardState[SDL_SCANCODE_LALT])
	{
		const float movement = mouseX * MOVEMENT_SENSITIVITY;
		m_Origin += m_Forward * movement;
	}
	else if (mouseState & SDL_BUTTON_LMASK && mouseState & SDL_BUTTON_RMASK)
	{
		const float movement = mouseY * MOVEMENT_SENSITIVITY;
		m_Origin -= m_Up * movement;
	}
	else if (mouseState & SDL_BUTTON_RMASK)
	{
		m_TotalYaw += mouseX * SENSITIVITY;
		m_TotalPitch -= mouseY * SENSITIVITY;
		const Matrix final{ Matrix::CreateRotationX(m_TotalPitch) * Matrix::CreateRotationY(m_TotalYaw) };

		m_Forward = final.TransformVector(Vector3::UnitZ);
		m_Forward.Normalize();
	}
	else if (mouseState & SDL_BUTTON_LMASK && !pKeyboardState[SDL_SCANCODE_LALT])
	{
		m_TotalYaw += mouseX * SENSITIVITY;
		const float movement = mouseY * MOVEMENT_SENSITIVITY;
		const Matrix final{ Matrix::CreateRotationX(m_TotalPitch) * Matrix::CreateRotationY(m_TotalYaw) };

		m_Origin.x += m_Forward.x * -movement;
		m_Origin.z += m_Forward.z * -movement;

		m_Forward = final.TransformVector(Vector3::UnitZ);
		m_Forward.Normalize();
	}

	if (mouseState & SDL_BUTTON_LMASK || mouseState & SDL_BUTTON_RMASK)
	{
		calculateCamMatrix = true;
	}

	// rotation
	if (pKeyboardState[SDL_SCANCODE_LALT] && mouseState & SDL_BUTTON_LMASK)
	{
		m_TotalYaw += mouseX * SENSITIVITY;
		m_TotalPitch -= mouseY * SENSITIVITY;
		const Matrix final{ Matrix::CreateRotationX(m_TotalPitch) * Matrix::CreateRotationY(m_TotalYaw) };

		m_Forward = final.TransformVector(Vector3::UnitZ);
		m_Forward.Normalize();

		CalculateViewMatrixTargetRotation();
		m_Origin = m_RotationOrigin;
	}

	//// clamp pitch
	constexpr float maxPitchAngle{85.f};
	m_TotalPitch = std::clamp(m_TotalPitch, -maxPitchAngle * TO_RADIANS, maxPitchAngle * TO_RADIANS);

	if (calculateCamMatrix)
	{
		m_Target = m_Origin + m_Forward * Vector3::Distance(m_Target, m_Origin);
		CalculateViewMatrix();
	}

	if (calculateProjectionMatrix)
	{
		calculateProjectionMatrix = false;
		CalculateProjectionMatrix();
	}
}

void Camera::UpdateFOV(float increment)
{
	m_FovAngle += increment;
	m_FovValue = tanf(m_FovAngle * TO_RADIANS / 2.f);
	CalculateProjectionMatrix();
}
