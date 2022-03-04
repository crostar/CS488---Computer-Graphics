#include <math.h>
#include "TrackBall.hpp"

using namespace glm;

void TrackBall::update(vec2 fNew, vec2 fOld, float fDiameter) {
  vec3 rotVec = calcRotVec(fNew, fOld, fDiameter);
  rotVec.y = - rotVec.y;
  updateAxisRotMatrix(rotVec);
  m_rotMat = glm::transpose(m_rotMat);
}

void TrackBall::reset() {
  m_rotMat = mat4(1.0f);
}

vec3 TrackBall::calcRotVec(vec2 fNew, vec2 fOld, float fDiameter) {
   long  nXOrigin, nYOrigin;
   vec3 fNewVec, fOldVec;
   float fLength;

   /* Vector pointing from center of virtual trackball to
    * new mouse position
    */
   fNewVec.x   = fNew.x * 2.0 / fDiameter;
   fNewVec.y   = fNew.y * 2.0 / fDiameter;
   fNewVec.z   = (1.0 - fNewVec.x * fNewVec.x - fNewVec.y * fNewVec.y);

   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fNewVec.z < 0.0) {
      fLength = sqrt(1.0 - fNewVec.z);
      fNewVec.z  = 0.0;
      fNewVec.x /= fLength;
      fNewVec.y /= fLength;
   } else {
      fNewVec.z = sqrt(fNewVec.z);
   }

   /* Vector pointing from center of virtual trackball to
    * old mouse position
    */
   fOldVec.x    = fOld.x * 2.0 / fDiameter;
   fOldVec.y    = fOld.y * 2.0 / fDiameter;
   fOldVec.z    = (1.0 - fOldVec.x * fOldVec.x - fOldVec.y * fOldVec.y);

   /* If the Z component is less than 0, the mouse point
    * falls outside of the trackball which is interpreted
    * as rotation about the Z axis.
    */
   if (fOldVec.z < 0.0) {
      fLength = sqrt(1.0 - fOldVec.z);
      fOldVec.z  = 0.0;
      fOldVec.x /= fLength;
      fOldVec.y /= fLength;
   } else {
      fOldVec.z = sqrt(fOldVec.z);
   }

   /* Generate rotation vector by calculating cross product:
    *
    * fOldVec x fNewVec.
    *
    * The rotation vector is the axis of rotation
    * and is non-unit length since the length of a crossproduct
    * is related to the angle between fOldVec and fNewVec which we need
    * in order to perform the rotation.
    */
   return glm::cross(fOldVec, fNewVec);
}

/*******************************************************
 * void vAxisRotMatrix(float fVecX, float fVecY, float fVecZ, Matrix mNewMat)
 *
 *    Calculate the rotation matrix for rotation about an arbitrary axis.
 *
 *    The axis of rotation is specified by (fVecX,fVecY,fVecZ). The length
 *    of the vector is the amount to rotate by.
 *
 * Parameters: fVecX,fVecY,fVecZ - Axis of rotation
 *             mNewMat - Output matrix of rotation in column-major format
 *                       (ie, translation components are in column 3 on rows
 *                       0,1, and 2).
 *
 *******************************************************/
void TrackBall::updateAxisRotMatrix(vec3 fVec) {
    float fRadians, fInvLength;

    /* Find the length of the vector which is the angle of rotation
     * (in radians)
     */
    fRadians = glm::length(fVec);

    /* If the vector has zero length - return the identity matrix */
    if (fRadians > -0.000001 && fRadians < 0.000001) {
        m_rotMat = mat4(1.0f);
        return;
    }

    /* Normalize the rotation vector now in preparation for making
     * rotation matrix.
     */
    fInvLength = 1 / fRadians;
    vec3 fNewVec = fInvLength * fVec;

    /* Create the arbitrary axis rotation matrix */
    double dSinAlpha = sin(fRadians);
    double dCosAlpha = cos(fRadians);
    double dT = 1 - dCosAlpha;

    m_rotMat[0][0] = dCosAlpha + fNewVec.x*fNewVec.x*dT;
    m_rotMat[0][1] = fNewVec.x*fNewVec.y*dT + fNewVec.z*dSinAlpha;
    m_rotMat[0][2] = fNewVec.x*fNewVec.z*dT - fNewVec.y*dSinAlpha;
    m_rotMat[0][3] = 0;

    m_rotMat[1][0] = fNewVec.x*fNewVec.y*dT - dSinAlpha*fNewVec.z;
    m_rotMat[1][1] = dCosAlpha + fNewVec.y*fNewVec.y*dT;
    m_rotMat[1][2] = fNewVec.y*fNewVec.z*dT + dSinAlpha*fNewVec.x;
    m_rotMat[1][3] = 0;

    m_rotMat[2][0] = fNewVec.z*fNewVec.x*dT + dSinAlpha*fNewVec.y;
    m_rotMat[2][1] = fNewVec.z*fNewVec.y*dT - dSinAlpha*fNewVec.x;
    m_rotMat[2][2] = dCosAlpha + fNewVec.z*fNewVec.z*dT;
    m_rotMat[2][3] = 0;

    m_rotMat[3][0] = 0;
    m_rotMat[3][1] = 0;
    m_rotMat[3][2] = 0;
    m_rotMat[3][3] = 1;
}
