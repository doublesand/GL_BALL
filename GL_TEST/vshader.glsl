//������ɫ����Ҫ�ǶԶ��������Ӧ������ϵ�任�ͷ���任

#version 330 core

in vec3 vPosition;  //����λ��
in vec3 vNormal;    //ÿ������ķ�����

// ��ת����
uniform mat4 rotationMatrix;
// ģ�ӱ任����
uniform mat4 modelViewMatrix;
// ͶӰ����
uniform mat4 projMatrix;

out vec3 N;   //������
out vec3 V;   //͸��ͶӰ

// Phong ����ģ�͵�ʵ�� (per-fragment shading)

void main()
{
	//һ�����������������ʾ����Ļ�ϣ���Ҫ���¼�������
	//1���������ģ������ϵת����������ϵ�������Ļ�������������ν�����λ��
	//2������������ϵת���������ϵ��������ô������۾�ѡ����һ���֣��������Լ�Ϊ���Ľ�һ������ϵ
	//3��ǰ�����������ģ�Ӿ������modelViewMatrix
	//4�����ǿ����Ķ�����ʵһ�Ż��棬Ҳ�������ǵý���ά��ͶӰ���Ӽ����У���ʱ����Ҫһ��ͶӰ����*ģ�Ӿ���
	//5�������Ӱ�ӵĻ���ô�����Ƚ�����ת���������ϵ��Ȼ�����ͶӰ����������Ӱ����

	vec4 v = projMatrix * modelViewMatrix * vec4(vPosition, 1.0);    //����ǰ���5������Ū����������һ����άƽ����
	
	//���ǵ���Ӱ�������Ҫ����͸�ӳ�������ʽ�����ģ�û�취
	vec4 point = vec4(v.xyz / v.w, 1.0);

	//ע�⣬��ת֮��ı仯�����������ϵ�½��еģ����������Ա仯����
	gl_Position = rotationMatrix * vec4(v.xyz / v.w, 1.0);  //�����������ת�任

	// ������任���������ϵ��
	vec4 vertPos_cameraspace = modelViewMatrix * vec4(vPosition, 1.0);

	// ���������任���������ϵ�²�����ƬԪ��ɫ�������ڹ�Դʲô�Ķ������������ϵ�´������Եý�����ת�Ƶ��������ϵ�´���
	N = (modelViewMatrix * vec4(vNormal, 0.0)).xyz;
	
	// �Զ���������͸�ӳ�������Ӱ����
	V = vertPos_cameraspace.xyz / vertPos_cameraspace.w;
}
