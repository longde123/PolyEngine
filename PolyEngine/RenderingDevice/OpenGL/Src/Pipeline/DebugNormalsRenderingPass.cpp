#include <PolyRenderingDeviceGLPCH.hpp>

#include <Pipeline/DebugNormalsRenderingPass.hpp>
#include <Proxy/GLMeshDeviceProxy.hpp>
#include <Proxy/GLTextureDeviceProxy.hpp>

using namespace Poly;

DebugNormalsRenderingPass::DebugNormalsRenderingPass(const GLRenderingDevice* rdi)
: RenderingPassBase(rdi, "Shaders/normals.vert.glsl", "Shaders/normals.frag.glsl")
{
}

void DebugNormalsRenderingPass::OnRun(Scene* world, const CameraComponent* camera, const AARect& /*rect*/, ePassType passType = ePassType::GLOBAL)
{
	GetProgram().BindProgram();
	const Matrix& mvp = camera->GetClipFromWorld();
	
	// Render meshes
	for (auto componentsTuple : world->IterateComponents<MeshRenderingComponent>())
	{
		const MeshRenderingComponent* meshCmp = std::get<MeshRenderingComponent*>(componentsTuple);
		const EntityTransform& trans = meshCmp->GetTransform();

		if (passType == ePassType::BY_MATERIAL &&
			(meshCmp->GetBlendingMode() == eBlendingMode::TRANSLUCENT || meshCmp->GetShadingModel() != eShadingMode::PBR))
		{
		 	continue;
		}

		const Matrix& objTransform = trans.GetWorldFromModel();
		Matrix screenTransform = mvp * objTransform;
		GetProgram().SetUniform("uTransform", objTransform);
		GetProgram().SetUniform("uMVPTransform", screenTransform);
		
		glPolygonMode(GL_FRONT_AND_BACK, meshCmp->GetIsWireframe() ? GL_LINE : GL_FILL);

		int i = 0;
		for (const MeshResource::SubMesh* subMesh : meshCmp->GetMesh()->GetSubMeshes())
		{
			const GLMeshDeviceProxy* meshProxy = static_cast<const GLMeshDeviceProxy*>(subMesh->GetMeshProxy());

			glBindVertexArray(meshProxy->GetVAO());

			glDrawElements(GL_TRIANGLES, (GLsizei)subMesh->GetMeshData().GetTriangleCount() * 3, GL_UNSIGNED_INT, NULL);
			glBindTexture(GL_TEXTURE_2D, 0);
			glBindVertexArray(0);

			++i;
		}

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	}
}
