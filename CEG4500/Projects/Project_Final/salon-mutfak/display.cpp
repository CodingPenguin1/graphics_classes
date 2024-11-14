void display(void) {
    recVec r;
    GLdouble ratio, radians, wd2, ndfl;
    GLdouble left, right, top, bottom, nearc, farc;

    //glDisable(GL_STENCIL_TEST);
    //glStencilMask(0x0);

    nearc = -gCamera.viewPos.z - gShapeSize * 0.5;
    if (nearc < 0.1)
        nearc = 0.1;
    farc = -gCamera.viewPos.z + gShapeSize * 0.5;

    // Misc stuff
    ratio = gCamera.screenWidth / (double)gCamera.screenHeight;
    radians = DTOR * gCamera.aperture / 2;
    wd2 = nearc * tan(radians);
    ndfl = nearc / gCamera.focalLength;

    // Derive the two eye positions
    CROSSPROD(gCamera.viewDir, gCamera.viewUp, r);
    normalise(&r);

    glDrawBuffer(GL_BACK_LEFT);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    if (gStereo) {
        left = -ratio * wd2 + 0.5 * gCamera.eyeSep * ndfl;
        right = ratio * wd2 + 0.5 * gCamera.eyeSep * ndfl;
    } else {
        left = -ratio * wd2;
        right = ratio * wd2;
    }
    top = wd2;
    bottom = -wd2;
    glFrustum(left, right, bottom, top, nearc, farc);

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    gluLookAt(gCamera.viewPos.x, gCamera.viewPos.y, gCamera.viewPos.z,
            gCamera.viewPos.x + gCamera.viewDir.x,
            gCamera.viewPos.y + gCamera.viewDir.y,
            gCamera.viewPos.z + gCamera.viewDir.z,
            gCamera.viewUp.x, gCamera.viewUp.y, gCamera.viewUp.z);

    // track ball rotation
    // translate object

    //glTranslatef(gCamera.viewPos.x, gCamera.viewPos.y, gCamera.viewPos.z);
    glRotatef(gTrackBallRotation[0], gTrackBallRotation[1], gTrackBallRotation[2], gTrackBallRotation[3]);
    //glTranslatef(-gCamera.viewPos.x, -gCamera.viewPos.y, -gCamera.viewPos.z);
    glRotatef(gWorldRotation[0], gWorldRotation[1], gWorldRotation[2], gWorldRotation[3]);

    if (gPolygons) {
        if (gLighting)
            glEnable(GL_LIGHTING);
        else
            glDisable(GL_LIGHTING);
        glCallList(gPolyList);
        glDisable(GL_LIGHTING);
    }
    DrawText(gCamera.screenWidth, gCamera.screenHeight);

    // #### MIRROR STUFF ####
    // Reference point on mirror
    static GLfloat reference_point[] = {252.589f, 105.177f, 172.436f};
    static GLfloat normal_vector[] = {0.00000f, 0.000000f, 1.000000f};

    // Set up stencil
    glEnable(GL_STENCIL_TEST);
    glStencilMask(0xFF);
    glClearStencil(0x00);
    glClear(GL_STENCIL_BUFFER_BIT);
    glStencilFunc(GL_NOTEQUAL, 0x1, 0x1);
    glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

    // Select mirror area for stencil
    mesh242();
    mesh243();

    glStencilFunc(GL_EQUAL, 0x1, 0x1);


    // Reflect the room
    //glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glScalef(1, 1, -1);
    glTranslatef(0, 0, -reference_point[2] * 2);

    // Render inside stencil
    glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

    glClear(GL_DEPTH_BUFFER_BIT);
    // mesh242();
    // mesh243();
    if (gPolygons) {
        if (gLighting)
            glEnable(GL_LIGHTING);
        else
            glDisable(GL_LIGHTING);
        glCallList(gPolyList);
        glDisable(GL_LIGHTING);
    }

    // Flip the room back
    glPopMatrix();

    // Disable stencil
    glDisable(GL_STENCIL_TEST);

    glutSwapBuffers();

    GL_ERROR;
}
