layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec2 v_uv;
layout (location = 3) in vec3 v_tangent;
layout (location = 4) in vec3 v_bitangent;

// uniform RendererUniforms Renderer;

out StandardShadingProperties Properties;


void CalculateTBNMatrix(vec3 normal)
{
    vec3 N = normalize(vec3(ViewModel * vec4(normal, 0.0f)));
    vec3 T = normalize(vec3(ViewModel * vec4(v_tangent, 0.0f)));
    vec3 B = normalize(vec3(ViewModel * vec4(v_bitangent, 0.0f)));
    // re-orthogonalize T with respect to N
    T = normalize(T - dot(T, N) * N);
    // then retrieve perpendicular vector B with the cross product of T and N
    B = cross(N, T);
    Properties.TBN = mat3(T,B,N);
}

void CalculateStandardProperties()
{
    Properties.N = normalize(ViewModel * vec4(v_normal, 0.0f));      //Surface normal
    Properties.ViewSpacePosition = ViewModel * vec4(v_position, 1.0f);
    Properties.V = -normalize(Properties.ViewSpacePosition); //Surface to eye direction
    Properties.L = -normalize(camera.View * vec4(directionalLight.Direction, 0.0f));      //Direction towards the light
    if(dot(Properties.N,Properties.V) < 0) Properties.N = -Properties.N;
    Properties.R = normalize(reflect(-Properties.L,Properties.N));
    Properties.H = normalize(Properties.L+Properties.V); 
    Properties.UV = v_uv;
    CalculateTBNMatrix(v_normal);
}
