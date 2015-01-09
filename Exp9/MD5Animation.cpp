#include <stdio.h>
#include "MD5.h"

MD5Animation::MD5Animation (void)
{
	frames=NULL;
	num_frames=0; num_joints=0;
}

void MD5Animation::Release(void)
{
	if (frames!=NULL) delete [] frames;
	frames=NULL;
}

int MD5Animation::Load(const char *file_name)
{
	FILE *fp=fopen(file_name, "rt");
	if (fp==NULL) return 0;

	int num_components;
	int *flag=NULL, *start_index=NULL;
	float *frame_data=NULL;
	MD5Joint *base_skeleton=NULL;

	char buf[512];
	char ctemp[256];
	int i, itemp;

	while (!feof(fp))
	{
		// ��ȡ�ļ�һ��
		fgets(buf, sizeof(buf), fp);

		// ����������ļ���
		if (1==sscanf(buf, "numFrames %d", &itemp))
		{
			// ���䶯��֡�洢�ռ�
			num_frames=itemp;
			frames=new MD5AnimationFrame [num_frames];
		}
		else if (1==sscanf(buf, "numJoints %d", &itemp))
		{
			// ����Ǽܴ洢�ռ�
			num_joints=itemp;
			flag=new int [num_joints];
			start_index=new int [num_joints];
			base_skeleton=new MD5Joint [num_joints];
			for (i=0; i<num_frames; ++i)
				frames[i].skeleton=new MD5Joint [num_joints];
		}
		else if (1==sscanf(buf, "frameRate %d", &itemp))
		{
			// ÿ�붯��֡��
			frame_rate=itemp;
		}
		else if (1==sscanf(buf, "numAnimatedComponents %d", &itemp))
		{
			// ���䶯��֡ѹ�����ݴ洢�ռ�
			num_components=itemp;
			if (num_components>0) frame_data=new float [num_components];
		}
		else if (strncmp(buf, "hierarchy {", 11)==0)
		{
			// ��ȡ�ؽ�����
			for (i=0; i<num_joints; ++i)
			{
				fgets(buf, sizeof(buf), fp);
				sscanf(buf, " %s %d %d %d", ctemp, 
					&(base_skeleton[i].parent), &(flag[i]), &(start_index[i]));
			}
		}
		else if (strncmp(buf, "bounds {", 8)==0)
		{
			// ��ȡ��Χ������
			for (i=0; i<num_frames; ++i)
			{
				fgets(buf, sizeof(buf), fp);
				CBoundingBox &b=frames[i].bbox;
				sscanf(buf, " ( %f %f %f ) ( %f %f %f )", 
					&b.min.x, &b.min.y, &b.min.z, &b.max.x, &b.max.y, &b.max.z);
			}
		}
		else if (strncmp(buf, "baseframe {", 11)==0)
		{
			// ��ȡ��׼֡�Ǽ�����
			for (i=0; i<num_joints; ++i)
			{
				fgets(buf, sizeof(buf), fp);
				MD5Joint &jt=base_skeleton[i];
				sscanf(buf, " ( %f %f %f ) ( %f %f %f )", 
					&(jt.pos.x), &(jt.pos.y), &(jt.pos.z), 
					&(jt.orient.x), &(jt.orient.y), &(jt.orient.z));
				jt.orient.CalculateScalar();
				jt.orient.w=-jt.orient.w;
			}
		}
		else if (1==sscanf(buf, "frame %d {", &itemp))
		{
			// ��ȡ����֡ѹ������
			for (i=0; i<num_components; ++i)
				fscanf(fp, "%f", &frame_data[i]);

			// ���춯��֡�����Ǽ���Ϣ
			BuildFrameSkeleton(num_components, flag, start_index, frame_data, 
				base_skeleton, frames[itemp].skeleton);
		}
	}

	if (flag!=NULL) delete [] flag;
	if (start_index!=NULL) delete [] start_index;
	if (frame_data!=NULL) delete [] frame_data;
	if (base_skeleton!=NULL) delete [] base_skeleton;

	fclose(fp);

	time_per_frame=1.0f/(float)frame_rate;
	total_time=time_per_frame*(float)num_frames;

	return 1;
}

void MD5Animation::BuildFrameSkeleton(int num_components, 
	int *flag, int *start_index, float *frame_data,
	MD5Joint *base_skeleton, MD5Joint *frame_skeleton)
// num_components: (����) ����֡ѹ����������
// flag: (����) �ؽڵ�ѹ����ʽ��־
// start_index: (����) �ؽ��ڶ���֡ѹ�������е���ʼλ������
// frame_data: (����) ����֡ѹ������
// base_skeleton: (����) ��׼֡�Ǽ�
// frame_skeleton: (���) ������Ķ���֡�Ǽ�
{
	int i, j;

	// ���춯��֡�Ǽ�
	memcpy(frame_skeleton, base_skeleton, num_joints*sizeof(MD5Joint));
	if (num_components==0) return;

	for (i=0; i<num_joints; ++i)
	{
		MD5Joint *pj=&frame_skeleton[i];
		j=start_index[i];
		if (flag[i] & 0x01) {pj->pos.x=frame_data[j]; ++j;}
		if (flag[i] & 0x02) {pj->pos.y=frame_data[j]; ++j;}
		if (flag[i] & 0x04) {pj->pos.z=frame_data[j]; ++j;}
		if (flag[i] & 0x08) {pj->orient.x=frame_data[j]; ++j;}
		if (flag[i] & 0x10) {pj->orient.y=frame_data[j]; ++j;}
		if (flag[i] & 0x20) {pj->orient.z=frame_data[j]; ++j;}

		pj->orient.CalculateScalar();
		pj->orient.w=-pj->orient.w;
	}

	// ���ؽڵ�λ�úͷ��������ڸ��ؽڵ����ֵ�任Ϊ���������еľ���ֵ
	char *processed=new char [num_joints]; // �ؽ��Ѵ����־
	memset(processed, 0, num_joints);
	
	for (i=0; i<num_joints; ++i)
		if (frame_skeleton[i].parent==-1) processed[i]=1;
	bool done=false;
	
	while (!done)
	{
		done=true;

		// ѭ�����еĹؽ�
		for (i=0; i<num_joints; ++i)
		{
			MD5Joint &jt=frame_skeleton[i];

			// ���ؽ��Ƿ�δ����
			if (!processed[i])
			{
				done=false;

				// ���ؽڵĸ��ؽ��Ƿ��Ѵ���
				if (processed[jt.parent])
				{
					processed[i]=1; // ���ؽڱ��Ϊ�Ѵ���

					// �任�ؽڵ�λ�úͷ���
					MD5Joint &jt_p=frame_skeleton[jt.parent];
					jt.orient=jt_p.orient*jt.orient;
					jt.pos=jt_p.orient.TransformPoint(jt.pos)+jt_p.pos;
				}
			}
		}
	}
	
	delete [] processed;
}

void MD5Animation::Animate(float time, MD5Joint *skeleton)
{
	int a,i;
	float t1,t2,u;
	a=(int)(time/time_per_frame);
	a=a%num_frames;
	if (a>num_frames-2) a=num_frames-2;
	t1=a*time_per_frame;
	t2=(a+1)*time_per_frame;
	u=(time-t1)/(t2-t1);
	for(i=0; i<num_joints; ++i)
	{
		skeleton[i].pos=(1-u)*frames[a].skeleton[i].pos+u*frames[a+1].skeleton[i].pos;
		skeleton[i].orient=QuaternionSlerp(frames[a].skeleton[i].orient,frames[a+1].skeleton[i].orient,u);
	}
}
