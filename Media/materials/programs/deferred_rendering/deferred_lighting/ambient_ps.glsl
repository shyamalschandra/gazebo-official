/******************************************************************************
Copyright (c) W.J. van der Laan

Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software  and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to use, 
copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the
Software, and to permit persons to whom the Software is furnished to do so, subject 
to the following conditions:

The above copyright notice and this permission notice shall be included in all copies 
or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, 
INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT 
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION 
OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,OUT OF OR IN CONNECTION WITH THE 
SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
******************************************************************************/
/** Deferred shading framework
	// W.J. :wumpus: van der Laan 2005 //
	
	Post shader: Multipass, ambient (base) pass
*/

//GL and DX expect a different value
arbfp1 float finalDepth(float4 p)
{
    // GL needs it in [0..1]
    return (p.z / p.w) * 0.5 + 0.5;
}

float finalDepth(float4 p)
{
    // normally it's in [-1..1]
    return p.z / p.w;
}

void main(
	float2 texCoord: TEXCOORD0, 
	float3 ray : TEXCOORD1,
	
	out float4 oColour : COLOR,
	out float oDepth : DEPTH,
	
	uniform sampler Tex0: register(s0),
	uniform float4x4 proj,
	uniform float4 ambientColor,
	uniform float3 farCorner,
	uniform float farClipDistance
	)
{
	float4 a0 = tex2D(Tex0, texCoord);  // Attribute 0: Normal+depth

	// Clip fragment if depth is too close, so the skybox can be rendered on the background
	clip(a0.w-0.0001);

	// Calculate ambient colour of fragment
	oColour = float4(ambientColor.rgb,0.0);

	// Calculate depth of fragment;
	float3 viewPos = normalize(ray) * farClipDistance * a0.w;
	float4 projPos = mul( proj, float4(viewPos, 1) );
	oDepth = finalDepth(projPos);
}
