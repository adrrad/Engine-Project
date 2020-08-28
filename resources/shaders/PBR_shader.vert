
void main()
{
    CalculateStandardProperties();
    gl_Position = MVP * vec4(v_position, 1.0);
}