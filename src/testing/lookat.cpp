
void lookat(Vec3f eye, Vec3f center, Vec3f up) {
    Vec3f t = (eye-center).normalize();
    Vec3f f = cross(up,t).normalize();
    Vec3f r = cross(t,f).normalize();
    Matrix Minv = Matrix::identity();
    Matrix Tr   = Matrix::identity();
    for (int i=0; i<3; i++) {
        Minv[0][i] = f[i];
        Minv[1][i] = r[i];
        Minv[2][i] = t[i];
        Tr[i][3] = -eye[i];
    }
    ModelView = Minv*Tr;
}
