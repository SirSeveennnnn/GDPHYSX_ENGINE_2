#include "ParticleWorld.h"

void ParticleWorld::startFrame()
{

}

void ParticleWorld::runPhysics(float deltaTime)
{
	registry.UpdateForces(deltaTime);

	integrate(deltaTime);

	generateContacts();
	

	if (contacts.size() > 0)
	{
		contactResolver->maxIterations = contacts.size() * 3;
		contactResolver->ResolveContacts(contacts, deltaTime);
	}

}

void ParticleWorld::integrate(float deltaTime)
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->Update(deltaTime);
	}

	for (int i = 0; i < boxParticles.size(); i++)
	{
		boxParticles[i]->Update(deltaTime);
	}
}

float ParticleWorld::Distance(glm::vec3 a, glm::vec3 b)
{
	return glm::length(a - b);
}

glm::vec3 ParticleWorld::GetBoxCenter()
{
	glm::vec3 center;
	center.x = boxParticles[0]->GetPosition().x;
	center.y = boxParticles[0]->GetPosition().y;
	center.z = boxParticles[0]->GetPosition().z;

	for (int i = 1; i < boxParticles.size(); i++)
	{
		center.x += boxParticles[i]->GetPosition().x;
		center.y += boxParticles[i]->GetPosition().y;
		center.z += boxParticles[i]->GetPosition().z;

		center.x /= 2;
		center.y /= 2;
		center.z /= 2;
	}

	return center;
}

void ParticleWorld::ResetBoxPosition()
{
	boxParticles[0]->SetPosition(glm::vec3(0, -40, -41));
	boxParticles[1]->SetPosition(glm::vec3(-14, -40, -55));
	boxParticles[2]->SetPosition(glm::vec3(0, -60, -41));
	boxParticles[3]->SetPosition(glm::vec3(-14, -60, -55));
	boxParticles[4]->SetPosition(glm::vec3(14, -40, -55));
	boxParticles[5]->SetPosition(glm::vec3(0, -40, -69));
	boxParticles[6]->SetPosition(glm::vec3(14, -60, -55));
	boxParticles[7]->SetPosition(glm::vec3(0, -60, -69));

	for (int i = 0; i < boxParticles.size(); i++)
	{
		boxParticles[i]->SetInverseMass(1);
		boxParticles[i]->isUsed = true;
		boxParticles[i]->SetLifeTime(250);
		boxParticles[i]->SetDamping(0.99f);
		boxParticles[i]->SetVelocity(glm::vec3(0, 0, 0));

	}

}

void ParticleWorld::DrawUpdate(MyShaders* shader, Camera camera)
{
	shader->UseShader();
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->Draw(shader->GetShaderProgram(), camera);
	}

	for (int i = 0; i < boxParticles.size(); i++)
	{
		boxParticles[i]->Draw(shader->GetShaderProgram(), camera);
	}
}

void ParticleWorld::initialize()
{
	for (int i = 0; i < particleLimit; i++)
	{
		particles.push_back(new Particle(glm::vec3(-50, -50, -30)));
	}

	//Creat Box
	// Box center: 0, -50, -55
	// corners distance should be 10

	boxParticles.push_back(new Particle(glm::vec3(0, -40, -41))); //Front Upper Right Particle    0
	boxParticles.push_back(new Particle(glm::vec3(-14, -40, -55))); //Front Upper Left Particle   1
	boxParticles.push_back(new Particle(glm::vec3(0, -60, -41))); //Front Lower Right Particle    2
	boxParticles.push_back(new Particle(glm::vec3(-14, -60, -55))); //Front Lower Left Particle   3

	boxParticles.push_back(new Particle(glm::vec3(14, -40, -55))); //Back Upper Right Particle    4
	boxParticles.push_back(new Particle(glm::vec3(0, -40, -69))); //Back Upper Left Particle      5
	boxParticles.push_back(new Particle(glm::vec3(14, -60, -55))); //Back Lower Right Particle    6
	boxParticles.push_back(new Particle(glm::vec3(0, -60, -69))); //Back Lower Left Particle      7

	//Connecting the rods
	//FRONT
	//UR - UL
	ParticleRod* UrUl = new ParticleRod();
	UrUl->particle[0] = boxParticles[0];
	UrUl->particle[1] = boxParticles[1];
	UrUl->length = Distance(boxParticles[0]->GetPosition(), boxParticles[1]->GetPosition());
	rods.push_back(UrUl);

	//LR - LL
	ParticleRod* LrLl = new ParticleRod();
	LrLl->particle[0] = boxParticles[2];
	LrLl->particle[1] = boxParticles[3];
	LrLl->length = Distance(boxParticles[2]->GetPosition(), boxParticles[3]->GetPosition());
	rods.push_back(LrLl);

	//UR - LR
	ParticleRod* UrLr = new ParticleRod();
	UrLr->particle[0] = boxParticles[0];
	UrLr->particle[1] = boxParticles[2];
	UrLr->length = Distance(boxParticles[0]->GetPosition(), boxParticles[2]->GetPosition());
	rods.push_back(UrLr);

	//UR - LR
	ParticleRod* UlLl = new ParticleRod();
	UlLl->particle[0] = boxParticles[1];
	UlLl->particle[1] = boxParticles[3];
	UlLl->length = Distance(boxParticles[1]->GetPosition(), boxParticles[3]->GetPosition());
	rods.push_back(UlLl);

	//BACK
	//B UR - UL
	ParticleRod* BUrUl = new ParticleRod();
	BUrUl->particle[0] = boxParticles[4];
	BUrUl->particle[1] = boxParticles[5];
	BUrUl->length = Distance(boxParticles[0]->GetPosition(), boxParticles[1]->GetPosition());
	rods.push_back(BUrUl);

	//B LR - LL
	ParticleRod* BLrLl = new ParticleRod();
	BLrLl->particle[0] = boxParticles[6];
	BLrLl->particle[1] = boxParticles[7];
	BLrLl->length = Distance(boxParticles[6]->GetPosition(), boxParticles[7]->GetPosition());
	rods.push_back(BLrLl);

	//B UR - LR
	ParticleRod* BUrLr = new ParticleRod();
	BUrLr->particle[0] = boxParticles[4];
	BUrLr->particle[1] = boxParticles[6];
	BUrLr->length = Distance(boxParticles[4]->GetPosition(), boxParticles[6]->GetPosition());
	rods.push_back(BUrLr);

	//B UL - LL
	ParticleRod* BUlLl = new ParticleRod();
	BUlLl->particle[0] = boxParticles[5];
	BUlLl->particle[1] = boxParticles[7];
	BUlLl->length = Distance(boxParticles[5]->GetPosition(), boxParticles[7]->GetPosition());
	rods.push_back(BUlLl);

	//Sides
	ParticleRod* side1 = new ParticleRod();
	side1->particle[0] = boxParticles[0];
	side1->particle[1] = boxParticles[4];
	side1->length = Distance(boxParticles[0]->GetPosition(), boxParticles[4]->GetPosition());
	rods.push_back(side1);

	ParticleRod* side2 = new ParticleRod();
	side2->particle[0] = boxParticles[1];
	side2->particle[1] = boxParticles[5];
	side2->length = Distance(boxParticles[1]->GetPosition(), boxParticles[5]->GetPosition());
	rods.push_back(side2);

	ParticleRod* side3 = new ParticleRod();
	side3->particle[0] = boxParticles[2];
	side3->particle[1] = boxParticles[6];
	side3->length = Distance(boxParticles[2]->GetPosition(), boxParticles[6]->GetPosition());
	rods.push_back(side3);

	ParticleRod* side4 = new ParticleRod();
	side4->particle[0] = boxParticles[3];
	side4->particle[1] = boxParticles[7];
	side4->length = Distance(boxParticles[3]->GetPosition(), boxParticles[7]->GetPosition());
	rods.push_back(side4);


	//Diagonal for stability
	/*
	ParticleRod* diagonal1 = new ParticleRod();
	diagonal1->particle[0] = boxParticles[0];
	diagonal1->particle[1] = boxParticles[7];
	diagonal1->length = Distance(boxParticles[0]->GetPosition(), boxParticles[7]->GetPosition());
	rods.push_back(diagonal1);

	ParticleRod* diagonal2 = new ParticleRod();
	diagonal2->particle[0] = boxParticles[1];
	diagonal2->particle[1] = boxParticles[6];
	diagonal2->length = Distance(boxParticles[1]->GetPosition(), boxParticles[6]->GetPosition());
	rods.push_back(diagonal2);

	ParticleRod* diagonal3 = new ParticleRod();
	diagonal3->particle[0] = boxParticles[2];
	diagonal3->particle[1] = boxParticles[5];
	diagonal3->length = Distance(boxParticles[2]->GetPosition(), boxParticles[5]->GetPosition());
	rods.push_back(diagonal3);

	ParticleRod* diagonal4 = new ParticleRod();
	diagonal4->particle[0] = boxParticles[3];
	diagonal4->particle[1] = boxParticles[4];
	diagonal4->length = Distance(boxParticles[3]->GetPosition(), boxParticles[4]->GetPosition());
	rods.push_back(diagonal4);
	*/

	//Extra Stability
	ParticleRod* front1 = new ParticleRod();
	front1->particle[0] = boxParticles[0];
	front1->particle[1] = boxParticles[3];
	front1->length = Distance(boxParticles[0]->GetPosition(), boxParticles[3]->GetPosition());
	rods.push_back(front1);

	ParticleRod* front2 = new ParticleRod();
	front2->particle[0] = boxParticles[1];
	front2->particle[1] = boxParticles[2];
	front2->length = Distance(boxParticles[1]->GetPosition(), boxParticles[2]->GetPosition());
	rods.push_back(front2);

	ParticleRod* back1 = new ParticleRod();
	back1->particle[0] = boxParticles[4];
	back1->particle[1] = boxParticles[7];
	back1->length = Distance(boxParticles[4]->GetPosition(), boxParticles[7]->GetPosition());
	rods.push_back(back1);

	ParticleRod* back2 = new ParticleRod();
	back2->particle[0] = boxParticles[5];
	back2->particle[1] = boxParticles[6];
	back2->length = Distance(boxParticles[5]->GetPosition(), boxParticles[6]->GetPosition());
	rods.push_back(back2);

	ParticleRod* left1 = new ParticleRod();
	left1->particle[0] = boxParticles[1];
	left1->particle[1] = boxParticles[7];
	left1->length = Distance(boxParticles[1]->GetPosition(), boxParticles[7]->GetPosition());
	rods.push_back(left1);

	ParticleRod* left2 = new ParticleRod();
	left2->particle[0] = boxParticles[3];
	left2->particle[1] = boxParticles[5];
	left2->length = Distance(boxParticles[3]->GetPosition(), boxParticles[5]->GetPosition());
	rods.push_back(left2);

	ParticleRod* right1 = new ParticleRod();
	right1->particle[0] = boxParticles[0];
	right1->particle[1] = boxParticles[6];
	right1->length = Distance(boxParticles[0]->GetPosition(), boxParticles[6]->GetPosition());
	rods.push_back(right1);

	ParticleRod* right2 = new ParticleRod();
	right2->particle[0] = boxParticles[2];
	right2->particle[1] = boxParticles[4];
	right2->length = Distance(boxParticles[2]->GetPosition(), boxParticles[4]->GetPosition());
	rods.push_back(right2);

	ParticleRod* top1 = new ParticleRod();
	top1->particle[0] = boxParticles[0];
	top1->particle[1] = boxParticles[5];
	top1->length = Distance(boxParticles[0]->GetPosition(), boxParticles[5]->GetPosition());
	rods.push_back(top1);

	ParticleRod* top2 = new ParticleRod();
	top2->particle[0] = boxParticles[1];
	top2->particle[1] = boxParticles[4];
	top2->length = Distance(boxParticles[1]->GetPosition(), boxParticles[4]->GetPosition());
	rods.push_back(top2);

	ParticleRod* bot1 = new ParticleRod();
	bot1->particle[0] = boxParticles[3];
	bot1->particle[1] = boxParticles[6];
	bot1->length = Distance(boxParticles[3]->GetPosition(), boxParticles[6]->GetPosition());
	rods.push_back(bot1);

	ParticleRod* bot2 = new ParticleRod();
	bot2->particle[0] = boxParticles[2];
	bot2->particle[1] = boxParticles[7];
	bot2->length = Distance(boxParticles[2]->GetPosition(), boxParticles[7]->GetPosition());
	rods.push_back(bot2);

	for (int i = 0; i < boxParticles.size(); i++)
	{
		boxParticles[i]->SetInverseMass(1);
		boxParticles[i]->isUsed = true;
		boxParticles[i]->SetLifeTime(250);
		boxParticles[i]->SetDamping(0.99f);
		boxParticles[i]->SetVelocity(glm::vec3(0, 0, 0));
		
	}

	glm::vec3 center = GetBoxCenter();
	cout << "Center X: " << center.x << " Y: " << center.y << " Z: " << center.z << endl;
}

void ParticleWorld::FreeMemory()
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->Destroy();
	}
}

void ParticleWorld::addContacts(Particle* a, Particle* b)
{
	//addcontact
	ParticleContact* contact = new ParticleContact();
	glm::vec3 dir = a->GetVelocity();
	float length = glm::length(dir);
	dir.x = dir.x / length;
	dir.y = dir.y / length;
	dir.z = dir.z / length;

	glm::vec3 dir2 = a->GetPosition() - b->GetPosition();
	float length2 = glm::length(dir2);
	dir2.x = dir2.x / length2;
	dir2.y = dir2.y / length2;
	dir2.z = dir2.z / length2;

	glm::vec3 finalDir = -dir + dir2;
	float finalLength = glm::length(finalDir);
	finalDir.x = finalDir.x / finalLength;
	finalDir.y = finalDir.y / finalLength;
	finalDir.z = finalDir.z / finalLength;
	

	contact->contactNormal = finalDir;//glm::vec3(-0.91, 0, 0.39);

	contact->restitution = 1;
	contact->particle[0] = a;
	contact->particle[1] = b;

	contacts.push_back(contact);

}

void ParticleWorld::generateContacts()
{
	//Collision detection with box
	contacts.clear();

	//Detect Particle To Box Collision
	for (int i = 0; i < particles.size(); i++)
	{
		glm::vec3 middle = GetBoxCenter();
		
		if (Distance(particles[i]->GetPosition(), middle) < 10.f && particles[i]->hasCollided == false)
		{
			cout << "Collision" << endl;
			particles[i]->hasCollided = true;

			//addContacts(particles[i], boxParticles[0]);
			for (int j = 0; j < boxParticles.size(); j++)
			{
				addContacts(particles[i], boxParticles[j]);
			}

		}
	}

	//cout << "Contacts Before: " << contacts.size() << endl;
	//Rod Contacts
	for (int i = 0; i < rods.size(); i++)
	{

		ParticleContact* contact = rods[i]->createContact();
		if (contact != nullptr)
		{
			
			contacts.push_back(contact);
		}
		
	}
	//cout << "Contacts After: " << contacts.size() << endl;
}

