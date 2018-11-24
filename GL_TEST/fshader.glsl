//ƬԪ��ɫ���Ĺ�����Ҫ�����ù�Դλ�ø�������

#version 330 core

in vec3 N;
in vec3 V;

uniform vec3 lightPos;
uniform int isShadow;

out vec4 fragmentColor;

void main()
{
	// ������ά����Ĳ�������
	vec3 ambiColor = vec3(0.4, 0.2, 0.2);          //������
	vec3 diffColor = vec3(0.5, 0.9, 0.9);          //������
	vec3 specColor = vec3(0.3, 0.6, 0.6);          //����

	//  ����N��L��V��R�ĸ���������һ������һ������Ϊ����Ƕȵ�cosֵ֮���
	vec3 N_norm = normalize(N);                   //���㷨����
	vec3 L_norm = normalize(lightPos - V);        //����ָ��⣬Ҳ�������߷���
	vec3 V_norm = normalize(-V);                  //��ͷ������ķ���,view
	vec3 R_norm = reflect(L_norm, N_norm);        //�����

	// ����������ϵ���;��淴��ϵ����clamp����˼���ڻ�ֵ��0��1֮ǰ�����Сѡ0�������ѡ1
	float lambertian = clamp(dot(L_norm, N_norm), 0.0, 1.0); // ֻ���Ƿ����������ϵĹ��ܽ���������
	float specular = clamp(dot(R_norm, V_norm), 0.0, 1.0);    //���淴�䣬ֻ�о��淴����V�����ϵķ������ܽ�������
	
	// �����Ƿ�Ϊ��Ӱѡ����ɫ
	if (isShadow == 1) 
	{
		fragmentColor = vec4(0.0, 0.0, 0.0, 1.0);
	}
	else 
	{ 
		//�����pow�е�ָ������߹�ָ�����߹�ָ��ԽС���߹�����Խ�󣬸߹�ָ���ӽ�����Ļ����Ǿ��ӵķ���
		fragmentColor = vec4(ambiColor + diffColor * lambertian + specColor * pow(specular, 10.0), 1.0);
	}
}
