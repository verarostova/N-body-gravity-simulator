#include <iostream>
#include <cmath>
#include <fstream>

using namespace std;
struct Vector3
{
    double x;
    double y;
    double z;
};

struct Body
{
    Vector3 position;
    Vector3 velocity;
    Vector3 acceleration;
    double mass;
};
int main()
{
    Body sun;
    Body earth;

    sun.mass = 1.0;
    earth.mass = 3.003e-6;

    earth.position.x = 1.0;
    earth.position.y = 0.0;
    earth.position.z = 0.0;

    earth.velocity.x = 0.0;
    earth.velocity.y = 2.0 * 3.141592653589793;
    earth.velocity.z = 0.0;

    sun.position.x = 0.0;
    sun.position.y = 0.0;
    sun.position.z = 0.0;

    sun.velocity.x = 0.0;
    sun.velocity.y = 0.0;
    sun.velocity.z = 0.0;

 
    double dt = 1.0 / 730.5;
    double G = 4.0 * 3.141592653589793 * 3.141592653589793;

    ofstream file("trajectory.csv");
    file << "time,x,y,z\n";
    double time = 0.0;

    file << time<<","
    << earth.position.x << ","
    << earth.position.y << "," 
    << earth.position.z << "\n";

    double r = sqrt(
        (sun.position.x - earth.position.x) * (sun.position.x - earth.position.x) +
        (sun.position.y - earth.position.y) * (sun.position.y - earth.position.y) +
        (sun.position.z - earth.position.z) * (sun.position.z - earth.position.z)
    );

    double v2 = earth.velocity.x * earth.velocity.x +
                earth.velocity.y * earth.velocity.y +
            earth.velocity.z * earth.velocity.z;

    double K = 0.5 * earth.mass * v2;
    double U = -G * sun.mass * earth.mass / r;
    double E0 = K + U;

    for (int i = 0; i < 730; i++)
{
    double dx = sun.position.x - earth.position.x;
    double dy = sun.position.y - earth.position.y;
    double dz = sun.position.z - earth.position.z;

    double r = sqrt(dx * dx + dy * dy + dz * dz);

    earth.acceleration.x = G * sun.mass * dx / (r * r * r);
    earth.acceleration.y = G * sun.mass * dy / (r * r * r);
    earth.acceleration.z = G * sun.mass * dz / (r * r * r);

    earth.velocity.x = earth.velocity.x + earth.acceleration.x * dt;
    earth.velocity.y = earth.velocity.y + earth.acceleration.y * dt;
    earth.velocity.z = earth.velocity.z + earth.acceleration.z * dt;

    earth.position.x = earth.position.x + earth.velocity.x * dt;
    earth.position.y = earth.position.y + earth.velocity.y * dt;
    earth.position.z = earth.position.z + earth.velocity.z * dt;
     
     time += dt;

    file << time << ","
     << earth.position.x << ","
     << earth.position.y << ","
     << earth.position.z << "\n";
    }
    
    file.close();

r = sqrt(
        (sun.position.x - earth.position.x) * (sun.position.x - earth.position.x) +
        (sun.position.y - earth.position.y) * (sun.position.y - earth.position.y) +
        (sun.position.z - earth.position.z) * (sun.position.z - earth.position.z)
    );

  v2 = earth.velocity.x * earth.velocity.x +
    earth.velocity.y * earth.velocity.y +
   earth.velocity.z * earth.velocity.z;

    K = 0.5 * earth.mass * v2;
    U = -G * sun.mass * earth.mass / r;

    double E1 = K + U;

    double relativeError = (E1 - E0) / abs(E0);

    cout << earth.position.x << " "
            << earth.position.y << " "
            << earth.position.z << endl;

    cout << earth.velocity.x << " "
            << earth.velocity.y << " "
            << earth.velocity.z << endl; 
    
    cout << earth.acceleration.x << endl;
    cout << earth.acceleration.y << endl;
    cout << earth.acceleration.z << endl;

    cout << "Initial energy: " << E0 << endl;
    cout << "Final energy: " << E1 << endl;
    cout << "Relative energy error: " << relativeError << endl;
}

