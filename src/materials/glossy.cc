/****************************************************************************
 * 		glossy_mat.cc: a glossy material based on Ashikhmin&Shirley's Paper
 *      This is part of the yafray package
 *      Copyright (C) 2006  Mathias Wein
 *
 *      This library is free software; you can redistribute it and/or
 *      modify it under the terms of the GNU Lesser General Public
 *      License as published by the Free Software Foundation; either
 *      version 2.1 of the License, or (at your option) any later version.
 *
 *      This library is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *      Lesser General Public License for more details.
 *
 *      You should have received a copy of the GNU Lesser General Public
 *      License along with this library; if not, write to the Free Software
 *      Foundation,Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#include <yafray_constants.h>
#include <yafraycore/nodematerial.h>
#include <core_api/environment.h>
#include <utilities/sample_utils.h>
#include <materials/microfacet.h>
#include <core_api/color_ramp.h>
#include <core_api/params.h>
#include <core_api/scene.h>

__BEGIN_YAFRAY

class glossyMat_t: public nodeMaterial_t
{
	public:
		glossyMat_t(const color_t &col, const color_t &dcol, float reflect, float diff, float expo, bool as_diffuse, visibility_t eVisibility = NORMAL_VISIBLE);
		virtual void initBSDF(const renderState_t &state, surfacePoint_t &sp, BSDF_t &bsdfTypes) const;
		virtual color_t eval(const renderState_t &state, const surfacePoint_t &sp, const vector3d_t &wo, const vector3d_t &wi, BSDF_t bsdfs, bool force_eval = false) const;
		virtual color_t sample(const renderState_t &state, const surfacePoint_t &sp, const vector3d_t &wo, vector3d_t &wi, sample_t &s, float &W) const;
		virtual float pdf(const renderState_t &state, const surfacePoint_t &sp, const vector3d_t &wo, const vector3d_t &wi, BSDF_t bsdfs) const;
		static material_t *factory(paraMap_t &, std::list< paraMap_t > &, renderEnvironment_t &);

		struct MDat_t
		{
			float mDiffuse, mGlossy, pDiffuse;
			void *stack;
		};

		void initOrenNayar(double sigma);

		virtual color_t getDiffuseColor(const renderState_t &state) const
		{
			MDat_t *dat = (MDat_t *)state.userdata;
			nodeStack_t stack(dat->stack);

			if(as_diffuse || with_diffuse) return (mDiffuseReflShader ? mDiffuseReflShader->getScalar(stack) : 1.f) * (diffuseS ? diffuseS->getColor(stack) : diff_color);
			else return color_t(0.f);
		}
		virtual color_t getGlossyColor(const renderState_t &state) const
		{
			MDat_t *dat = (MDat_t *)state.userdata;
			nodeStack_t stack(dat->stack);

			return (glossyRefS ? glossyRefS->getScalar(stack) : reflectivity) * (glossyS ? glossyS->getColor(stack) : gloss_color);
		}

	private:
		float OrenNayar(const vector3d_t &wi, const vector3d_t &wo, const vector3d_t &N, bool useTextureSigma, double textureSigma) const;

	protected:
		shaderNode_t *diffuseS = nullptr;
		shaderNode_t *glossyS = nullptr;
		shaderNode_t *glossyRefS = nullptr;
		shaderNode_t *bumpS = nullptr;
		shaderNode_t *exponentS = nullptr;
		shaderNode_t *mWireFrameShader = nullptr;     //!< Shader node for wireframe shading (float)
		shaderNode_t *mSigmaOrenShader = nullptr;     //!< Shader node for sigma in Oren Nayar material
		shaderNode_t *mDiffuseReflShader = nullptr;   //!< Shader node for diffuse reflection strength (float)
		color_t gloss_color, diff_color;
		float exponent, exp_u, exp_v;
		float reflectivity;
		float mDiffuse;
		bool as_diffuse, with_diffuse = false, anisotropic = false;
		bool orenNayar;
		float orenA, orenB;
};

glossyMat_t::glossyMat_t(const color_t &col, const color_t &dcol, float reflect, float diff, float expo, bool as_diff, visibility_t eVisibility):
	gloss_color(col), diff_color(dcol), exponent(expo), reflectivity(reflect), mDiffuse(diff), as_diffuse(as_diff)
{
	mVisibility = eVisibility;
	bsdfFlags = BSDF_NONE;

	if(diff > 0)
	{
		bsdfFlags = BSDF_DIFFUSE | BSDF_REFLECT;
		with_diffuse = true;
	}

	orenNayar = false;

	bsdfFlags |= as_diffuse ? (BSDF_DIFFUSE | BSDF_REFLECT) : (BSDF_GLOSSY | BSDF_REFLECT);

	mVisibility = eVisibility;
}

void glossyMat_t::initBSDF(const renderState_t &state, surfacePoint_t &sp, BSDF_t &bsdfTypes) const
{
	MDat_t *dat = (MDat_t *)state.userdata;
	dat->stack = (char *)state.userdata + sizeof(MDat_t);
	nodeStack_t stack(dat->stack);
	if(bumpS) evalBump(stack, state, sp, bumpS);

	auto end = allViewindep.end();
	for(auto iter = allViewindep.begin(); iter != end; ++iter)(*iter)->eval(stack, state, sp);
	bsdfTypes = bsdfFlags;
	dat->mDiffuse = mDiffuse;
	dat->mGlossy = glossyRefS ? glossyRefS->getScalar(stack) : reflectivity;
	dat->pDiffuse = std::min(0.6f, 1.f - (dat->mGlossy / (dat->mGlossy + (1.f - dat->mGlossy) * dat->mDiffuse)));
}

void glossyMat_t::initOrenNayar(double sigma)
{
	double sigma2 = sigma * sigma;
	orenA = 1.0 - 0.5 * (sigma2 / (sigma2 + 0.33));
	orenB = 0.45 * sigma2 / (sigma2 + 0.09);
	orenNayar = true;
}

float glossyMat_t::OrenNayar(const vector3d_t &wi, const vector3d_t &wo, const vector3d_t &N, bool useTextureSigma, double textureSigma) const
{
	float cos_ti = std::max(-1.f, std::min(1.f, N * wi));
	float cos_to = std::max(-1.f, std::min(1.f, N * wo));
	float maxcos_f = 0.f;

	if(cos_ti < 0.9999f && cos_to < 0.9999f)
	{
		vector3d_t v1 = (wi - N * cos_ti).normalize();
		vector3d_t v2 = (wo - N * cos_to).normalize();
		maxcos_f = std::max(0.f, v1 * v2);
	}

	float sin_alpha, tan_beta;

	if(cos_to >= cos_ti)
	{
		sin_alpha = fSqrt(1.f - cos_ti * cos_ti);
		tan_beta = fSqrt(1.f - cos_to * cos_to) / ((cos_to == 0.f) ? 1e-8f : cos_to); // white (black on windows) dots fix for oren-nayar, could happen with bad normals
	}
	else
	{
		sin_alpha = fSqrt(1.f - cos_to * cos_to);
		tan_beta = fSqrt(1.f - cos_ti * cos_ti) / ((cos_ti == 0.f) ? 1e-8f : cos_ti); // white (black on windows) dots fix for oren-nayar, could happen with bad normals
	}

	if(useTextureSigma)
	{
		double sigma_squared = textureSigma * textureSigma;
		double mOrenNayar_TextureA = 1.0 - 0.5 * (sigma_squared / (sigma_squared + 0.33));
		double mOrenNayar_TextureB = 0.45 * sigma_squared / (sigma_squared + 0.09);
		return std::min(1.f, std::max(0.f, (float)(mOrenNayar_TextureA + mOrenNayar_TextureB * maxcos_f * sin_alpha * tan_beta)));
	}
	else
	{
		return std::min(1.f, std::max(0.f, (float)(orenA + orenB * maxcos_f * sin_alpha * tan_beta)));
	}
}

color_t glossyMat_t::eval(const renderState_t &state, const surfacePoint_t &sp, const vector3d_t &wo, const vector3d_t &wi, BSDF_t bsdfs, bool force_eval) const
{
	if(!force_eval)	//If the flag force_eval = true then the next line will be skipped, necessary for the Glossy Direct render pass
	{
		if(!(bsdfs & BSDF_DIFFUSE) || ((sp.Ng * wi) * (sp.Ng * wo)) < 0.f) return color_t(0.f);
	}

	MDat_t *dat = (MDat_t *)state.userdata;
	color_t col(0.f);
	bool diffuse_flag = bsdfs & BSDF_DIFFUSE;

	nodeStack_t stack(dat->stack);
	vector3d_t N = FACE_FORWARD(sp.Ng, sp.N, wo);

	float wiN = std::fabs(wi * N);
	float woN = std::fabs(wo * N);


	if((as_diffuse && diffuse_flag) || (!as_diffuse && (bsdfs & BSDF_GLOSSY)))
	{
		vector3d_t H = (wo + wi).normalize(); // half-angle
		float cos_wi_H = std::max(0.f, wi * H);
		float glossy;

		if(anisotropic)
		{
			vector3d_t Hs(H * sp.NU, H * sp.NV, H * N);
			glossy = AS_Aniso_D(Hs, exp_u, exp_v) * SchlickFresnel(cos_wi_H, dat->mGlossy) / ASDivisor(cos_wi_H, woN, wiN);
		}
		else
		{
			glossy = Blinn_D(H * N, (exponentS ? exponentS->getScalar(stack) : exponent)) * SchlickFresnel(cos_wi_H, dat->mGlossy) / ASDivisor(cos_wi_H, woN, wiN);

		}

		col = glossy * (glossyS ? glossyS->getColor(stack) : gloss_color);
	}

	if(with_diffuse && diffuse_flag)
	{
		color_t addCol = dat->mDiffuse * (1.f - dat->mGlossy) * (diffuseS ? diffuseS->getColor(stack) : diff_color);

		if(mDiffuseReflShader) addCol *= mDiffuseReflShader->getScalar(stack);

		if(orenNayar)
		{
			double textureSigma = (mSigmaOrenShader ? mSigmaOrenShader->getScalar(stack) : 0.f);
			bool useTextureSigma = (mSigmaOrenShader ? true : false);

			addCol *= OrenNayar(wi, wo, N, useTextureSigma, textureSigma);
		}

		col += addCol;

		//diffuseReflect(wiN, woN, dat->mGlossy, dat->mDiffuse, (diffuseS ? diffuseS->getColor(stack) : diff_color)) * ((orenNayar)?OrenNayar(wi, wo, N):1.f);
	}

	float wireFrameAmount = (mWireFrameShader ? mWireFrameShader->getScalar(stack) * mWireFrameAmount : mWireFrameAmount);
	applyWireFrame(col, wireFrameAmount, sp);
	return col;
}


color_t glossyMat_t::sample(const renderState_t &state, const surfacePoint_t &sp, const vector3d_t &wo, vector3d_t &wi, sample_t &s, float &W) const
{
	MDat_t *dat = (MDat_t *)state.userdata;
	float cos_Ng_wo = sp.Ng * wo;
	float cos_Ng_wi;
	vector3d_t N = FACE_FORWARD(sp.Ng, sp.N, wo);//(cos_Ng_wo < 0) ? -sp.N : sp.N;
	vector3d_t Hs;
	s.pdf = 0.f;
	float wiN = 0.f;
	float woN = std::fabs(wo * N);
	float cos_wo_H = 0.f;

	color_t scolor(0.f);

	float s1 = s.s1;
	float cur_pDiffuse = dat->pDiffuse;
	bool use_glossy = as_diffuse ? (s.flags & BSDF_DIFFUSE) : (s.flags & BSDF_GLOSSY);
	bool use_diffuse = with_diffuse && (s.flags & BSDF_DIFFUSE);
	nodeStack_t stack(dat->stack);
	float glossy = 0.f;

	if(use_diffuse)
	{
		float s_pDiffuse = use_glossy ? cur_pDiffuse : 1.f;
		if(s1 < s_pDiffuse)
		{
			s1 /= s_pDiffuse;
			wi = SampleCosHemisphere(N, sp.NU, sp.NV, s1, s.s2);

			cos_Ng_wi = sp.Ng * wi;

			if(cos_Ng_wi * cos_Ng_wo < 0.f)
			{
				float wireFrameAmount = (mWireFrameShader ? mWireFrameShader->getScalar(stack) * mWireFrameAmount : mWireFrameAmount);
				applyWireFrame(scolor, wireFrameAmount, sp);
				return scolor;
			}

			wiN = std::fabs(wi * N);

			s.pdf = wiN;

			if(use_glossy)
			{
				vector3d_t H = (wi + wo).normalize();
				cos_wo_H = wo * H;
				float cos_wi_H = std::fabs(wi * H);
				float cos_N_H = N * H;
				if(anisotropic)
				{
					vector3d_t Hs(H * sp.NU, H * sp.NV, cos_N_H);
					s.pdf = s.pdf * cur_pDiffuse + AS_Aniso_Pdf(Hs, cos_wo_H, exp_u, exp_v) * (1.f - cur_pDiffuse);
					glossy = AS_Aniso_D(Hs, exp_u, exp_v) * SchlickFresnel(cos_wi_H, dat->mGlossy) / ASDivisor(cos_wi_H, woN, wiN);
				}
				else
				{
					s.pdf = s.pdf * cur_pDiffuse + Blinn_Pdf(cos_N_H, cos_wo_H, (exponentS ? exponentS->getScalar(stack) : exponent)) * (1.f - cur_pDiffuse);
					glossy = Blinn_D(cos_N_H, (exponentS ? exponentS->getScalar(stack) : exponent)) * SchlickFresnel(cos_wi_H, dat->mGlossy) / ASDivisor(cos_wi_H, woN, wiN);
				}
			}
			s.sampledFlags = BSDF_DIFFUSE | BSDF_REFLECT;

			if(!(s.flags & BSDF_REFLECT))
			{
				scolor = color_t(0.f);
				float wireFrameAmount = (mWireFrameShader ? mWireFrameShader->getScalar(stack) * mWireFrameAmount : mWireFrameAmount);
				applyWireFrame(scolor, wireFrameAmount, sp);
				return scolor;
			}

			scolor = glossy * (glossyS ? glossyS->getColor(stack) : gloss_color);

			if(use_diffuse)
			{
				color_t addCol = diffuseReflect(wiN, woN, dat->mGlossy, dat->mDiffuse, (diffuseS ? diffuseS->getColor(stack) : diff_color));

				if(mDiffuseReflShader) addCol *= mDiffuseReflShader->getScalar(stack);

				if(orenNayar)
				{
					double textureSigma = (mSigmaOrenShader ? mSigmaOrenShader->getScalar(stack) : 0.f);
					bool useTextureSigma = (mSigmaOrenShader ? true : false);

					addCol *= OrenNayar(wi, wo, N, useTextureSigma, textureSigma);
				}
				scolor += addCol;
			}
			W = wiN / (s.pdf * 0.99f + 0.01f);

			float wireFrameAmount = (mWireFrameShader ? mWireFrameShader->getScalar(stack) * mWireFrameAmount : mWireFrameAmount);
			applyWireFrame(scolor, wireFrameAmount, sp);
			return scolor;
		}
		s1 -= cur_pDiffuse;
		s1 /= (1.f - cur_pDiffuse);
	}

	if(use_glossy)
	{
		if(anisotropic)
		{
			AS_Aniso_Sample(Hs, s1, s.s2, exp_u, exp_v);
			vector3d_t H = Hs.x * sp.NU + Hs.y * sp.NV + Hs.z * N;
			cos_wo_H = wo * H;
			if(cos_wo_H < 0.f)
			{
				H.reflect(N);
				cos_wo_H = wo * H;
			}
			// Compute incident direction by reflecting wo about H
			wi = reflect_dir(H, wo);
			cos_Ng_wi = sp.Ng * wi;

			if(cos_Ng_wo * cos_Ng_wi < 0.f)
			{
				scolor = color_t(0.f);
				float wireFrameAmount = (mWireFrameShader ? mWireFrameShader->getScalar(stack) * mWireFrameAmount : mWireFrameAmount);
				applyWireFrame(scolor, wireFrameAmount, sp);
				return scolor;
			}

			wiN = std::fabs(wi * N);

			s.pdf = AS_Aniso_Pdf(Hs, cos_wo_H, exp_u, exp_v);
			glossy = AS_Aniso_D(Hs, exp_u, exp_v) * SchlickFresnel(cos_wo_H, dat->mGlossy) / ASDivisor(cos_wo_H, woN, wiN);
		}
		else
		{
			Blinn_Sample(Hs, s1, s.s2, (exponentS ? exponentS->getScalar(stack) : exponent));
			vector3d_t H = Hs.x * sp.NU + Hs.y * sp.NV + Hs.z * N;
			cos_wo_H = wo * H;
			if(cos_wo_H < 0.f)
			{
				H.reflect(N);
				cos_wo_H = wo * H;
			}
			// Compute incident direction by reflecting wo about H
			wi = reflect_dir(H, wo);
			cos_Ng_wi = sp.Ng * wi;

			if(cos_Ng_wo * cos_Ng_wi < 0.f)
			{
				scolor = color_t(0.f);
				float wireFrameAmount = (mWireFrameShader ? mWireFrameShader->getScalar(stack) * mWireFrameAmount : mWireFrameAmount);
				applyWireFrame(scolor, wireFrameAmount, sp);
				return scolor;
			}

			wiN = std::fabs(wi * N);
			float cosHN = H * N;

			s.pdf = Blinn_Pdf(cosHN, cos_wo_H, (exponentS ? exponentS->getScalar(stack) : exponent));
			glossy = Blinn_D(cosHN, (exponentS ? exponentS->getScalar(stack) : exponent)) * SchlickFresnel(cos_wo_H, dat->mGlossy)  / ASDivisor(cos_wo_H, woN, wiN);
		}

		scolor = glossy * (glossyS ? glossyS->getColor(stack) : gloss_color);
		s.sampledFlags = as_diffuse ? BSDF_DIFFUSE | BSDF_REFLECT : BSDF_GLOSSY | BSDF_REFLECT;
	}

	if(use_diffuse)
	{
		color_t addCol = diffuseReflect(wiN, woN, dat->mGlossy, dat->mDiffuse, (diffuseS ? diffuseS->getColor(stack) : diff_color));

		if(mDiffuseReflShader) addCol *= mDiffuseReflShader->getScalar(stack);

		if(orenNayar)
		{
			double textureSigma = (mSigmaOrenShader ? mSigmaOrenShader->getScalar(stack) : 0.f);
			bool useTextureSigma = (mSigmaOrenShader ? true : false);

			addCol *= OrenNayar(wi, wo, N, useTextureSigma, textureSigma);
		}
		s.pdf = wiN * cur_pDiffuse + s.pdf * (1.f - cur_pDiffuse);
		scolor += addCol;
	}

	W = wiN / (s.pdf * 0.99f + 0.01f);

	float wireFrameAmount = (mWireFrameShader ? mWireFrameShader->getScalar(stack) * mWireFrameAmount : mWireFrameAmount);
	applyWireFrame(scolor, wireFrameAmount, sp);
	return scolor;
}

float glossyMat_t::pdf(const renderState_t &state, const surfacePoint_t &sp, const vector3d_t &wo, const vector3d_t &wi, BSDF_t flags) const
{
	MDat_t *dat = (MDat_t *)state.userdata;
	nodeStack_t stack(dat->stack);

	if((sp.Ng * wo) * (sp.Ng * wi) < 0.f) return 0.f;
	vector3d_t N = FACE_FORWARD(sp.Ng, sp.N, wo);
	float pdf = 0.f;
	float cos_wo_H = 0.f;
	float cos_N_H = 0.f;

	float cur_pDiffuse = dat->pDiffuse;
	bool use_glossy = as_diffuse ? (flags & BSDF_DIFFUSE) : (flags & BSDF_GLOSSY);
	bool use_diffuse = with_diffuse && (flags & BSDF_DIFFUSE);

	if(use_diffuse)
	{
		pdf = std::fabs(wi * N);
		if(use_glossy)
		{
			vector3d_t H = (wi + wo).normalize();
			cos_wo_H = wo * H;
			cos_N_H = N * H;
			if(anisotropic)
			{
				vector3d_t Hs(H * sp.NU, H * sp.NV, cos_N_H);
				pdf = pdf * cur_pDiffuse + AS_Aniso_Pdf(Hs, cos_wo_H, exp_u, exp_v) * (1.f - cur_pDiffuse);
			}
			else pdf = pdf * cur_pDiffuse + Blinn_Pdf(cos_N_H, cos_wo_H, (exponentS ? exponentS->getScalar(stack) : exponent)) * (1.f - cur_pDiffuse);
		}
		return pdf;
	}

	if(use_glossy)
	{
		vector3d_t H = (wi + wo).normalize();
		cos_wo_H = wo * H;
		cos_N_H = N * H;
		if(anisotropic)
		{
			vector3d_t Hs(H * sp.NU, H * sp.NV, cos_N_H);
			pdf = AS_Aniso_Pdf(Hs, cos_wo_H, exp_u, exp_v);
		}
		else pdf = Blinn_Pdf(cos_N_H, cos_wo_H, (exponentS ? exponentS->getScalar(stack) : exponent));
	}
	return pdf;
}

material_t *glossyMat_t::factory(paraMap_t &params, std::list< paraMap_t > &paramList, renderEnvironment_t &render)
{
	color_t col(1.f), dcol(1.f);
	float refl = 1.f;
	float diff = 0.f;
	float exponent = 50.f; //wild guess, do sth better
	bool as_diff = true;
	bool aniso = false;
	std::string sVisibility = "normal";
	visibility_t visibility = NORMAL_VISIBLE;
	int mat_pass_index = 0;
	bool receive_shadows = true;
	int additionaldepth = 0;
	float samplingfactor = 1.f;
	float WireFrameAmount = 0.f;           //!< Wireframe shading amount
	float WireFrameThickness = 0.01f;      //!< Wireframe thickness
	float WireFrameExponent = 0.f;         //!< Wireframe exponent (0.f = solid, 1.f=linearly gradual, etc)
	color_t WireFrameColor = color_t(1.f); //!< Wireframe shading color

	const std::string *name = nullptr;
	params.getParam("color", col);
	params.getParam("diffuse_color", dcol);
	params.getParam("diffuse_reflect", diff);
	params.getParam("glossy_reflect", refl);
	params.getParam("as_diffuse", as_diff);
	params.getParam("exponent", exponent);
	params.getParam("anisotropic", aniso);

	params.getParam("receive_shadows", receive_shadows);
	params.getParam("visibility", sVisibility);
	params.getParam("mat_pass_index",   mat_pass_index);
	params.getParam("additionaldepth",   additionaldepth);
	params.getParam("samplingfactor",   samplingfactor);

	params.getParam("wireframe_amount",  WireFrameAmount);
	params.getParam("wireframe_thickness",  WireFrameThickness);
	params.getParam("wireframe_exponent",  WireFrameExponent);
	params.getParam("wireframe_color",  WireFrameColor);

	if(sVisibility == "normal") visibility = NORMAL_VISIBLE;
	else if(sVisibility == "no_shadows") visibility = VISIBLE_NO_SHADOWS;
	else if(sVisibility == "shadow_only") visibility = INVISIBLE_SHADOWS_ONLY;
	else if(sVisibility == "invisible") visibility = INVISIBLE;
	else visibility = NORMAL_VISIBLE;

	glossyMat_t *mat = new glossyMat_t(col, dcol, refl, diff, exponent, as_diff, visibility);

	mat->setMaterialIndex(mat_pass_index);
	mat->mReceiveShadows = receive_shadows;
	mat->additionalDepth = additionaldepth;

	mat->mWireFrameAmount = WireFrameAmount;
	mat->mWireFrameThickness = WireFrameThickness;
	mat->mWireFrameExponent = WireFrameExponent;
	mat->mWireFrameColor = WireFrameColor;

	mat->setSamplingFactor(samplingfactor);

	if(aniso)
	{
		float e_u = 50.0, e_v = 50.0;
		params.getParam("exp_u", e_u);
		params.getParam("exp_v", e_v);
		mat->anisotropic = true;
		mat->exp_u = e_u;
		mat->exp_v = e_v;
	}

	if(params.getParam("diffuse_brdf", name))
	{
		if(*name == "Oren-Nayar")
		{
			double sigma = 0.1;
			params.getParam("sigma", sigma);
			mat->initOrenNayar(sigma);
		}
	}

	std::vector<shaderNode_t *> roots;
	std::map<std::string, shaderNode_t *> nodeList;

	// Prepare our node list
	nodeList["diffuse_shader"] = nullptr;
	nodeList["glossy_shader"] = nullptr;
	nodeList["glossy_reflect_shader"] = nullptr;
	nodeList["bump_shader"] = nullptr;
	nodeList["sigma_oren_shader"]   = nullptr;
	nodeList["exponent_shader"] = nullptr;
	nodeList["wireframe_shader"]    = nullptr;
	nodeList["diffuse_refl_shader"] = nullptr;

	if(mat->loadNodes(paramList, render))
	{
		mat->parseNodes(params, roots, nodeList);
	}
	else Y_ERROR << "Glossy: loadNodes() failed!" << yendl;

	mat->diffuseS = nodeList["diffuse_shader"];
	mat->glossyS = nodeList["glossy_shader"];
	mat->glossyRefS = nodeList["glossy_reflect_shader"];
	mat->bumpS = nodeList["bump_shader"];
	mat->mSigmaOrenShader = nodeList["sigma_oren_shader"];
	mat->exponentS = nodeList["exponent_shader"];
	mat->mWireFrameShader    = nodeList["wireframe_shader"];
	mat->mDiffuseReflShader  = nodeList["diffuse_refl_shader"];

	// solve nodes order
	if(!roots.empty())
	{
		std::vector<shaderNode_t *> colorNodes;

		mat->solveNodesOrder(roots);

		if(mat->diffuseS) mat->getNodeList(mat->diffuseS, colorNodes);
		if(mat->glossyS) mat->getNodeList(mat->glossyS, colorNodes);
		if(mat->glossyRefS) mat->getNodeList(mat->glossyRefS, colorNodes);
		if(mat->mSigmaOrenShader)    mat->getNodeList(mat->mSigmaOrenShader, colorNodes);
		if(mat->exponentS) mat->getNodeList(mat->exponentS, colorNodes);
		if(mat->mWireFrameShader)    mat->getNodeList(mat->mWireFrameShader, colorNodes);
		if(mat->mDiffuseReflShader)  mat->getNodeList(mat->mDiffuseReflShader, colorNodes);
		mat->filterNodes(colorNodes, mat->allViewdep, VIEW_DEP);
		mat->filterNodes(colorNodes, mat->allViewindep, VIEW_INDEP);
		if(mat->bumpS) mat->getNodeList(mat->bumpS, mat->bumpNodes);
	}

	mat->reqMem = mat->reqNodeMem + sizeof(MDat_t);

	return mat;
}

extern "C"
{
	YAFRAYPLUGIN_EXPORT void registerPlugin(renderEnvironment_t &render)
	{
		render.registerFactory("glossy", glossyMat_t::factory);
	}
}

__END_YAFRAY
