#pragma once
using namespace dae;

class Camera
{
public:
	Camera() = default;

	Camera(const Vector3& origin, float fovAngle, float aspectRatio, Vector3 target = {0,0,0});

	Matrix& GetViewMatrix() { return m_ViewMatrix; }
	Matrix& GetInvViewMatrix() { return m_InvViewMatrix; }
	Matrix& GetProjectionMatrix() { return m_ProjectionMatrix; }

	void Update(const Timer* pTimer);

	void UpdateFOV(float increment);

private:
	Vector3 m_Origin{};
	Vector3 m_Target{};
	Vector3 m_RotationOrigin{ m_Target };
	Vector3 m_StarTarget{ m_Target };
	float m_FovAngle{ 45.f };
	float m_FovValue{ tanf((m_FovAngle * TO_RADIANS) / 2.f) };

	float m_AspectRatio{};

	Vector3 m_Forward{ Vector3::UnitZ };
	Vector3 m_Up{ Vector3::UnitY };
	Vector3 m_Right{ Vector3::UnitX };

	float m_TotalYaw{};
	float m_TotalPitch{};

	Matrix m_InvViewMatrix{};
	Matrix m_ViewMatrix{};
	Matrix m_ProjectionMatrix{};

	float m_NearPlane{ 0.1f };
	float m_FarPlane{ 1000.f };

	float targetStopDistance{ };

	void CalculateViewMatrix();
	void CalculateViewMatrixTargetRotation();
	void CalculateProjectionMatrix();

};

