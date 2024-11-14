/*
 * Solution for project 0 of the course CEG476/676 taught by Thomas
 * Wischgoll.
 *
 * This solution kept simple and can be used as starting point for the
 * following projects. It creates a top, side, and front view of the
 * geometry retreived from a ply file using Greg Turk's ply reader.
 *
 * Thomas Wischgoll, October 2007.
 */


#include <stdlib.h>
#include <math.h>
#include <GL/glut.h>

#include "ply.h"

#include <iostream>

using std::cout;
using std::endl;

#include <math.h>


// ply vertices and faces
Vertex **vlist;
Face **flist;
int num_elems;

/******************************************************************************
Read in a PLY file.
******************************************************************************/

void read_test(char *filename)
{
  int i,j,k;
  PlyFile *ply;
  int nelems;
  char **elist;
  int file_type;
  float version;
  int nprops;
  PlyProperty **plist;
  char *elem_name;
  int num_comments;
  char **comments;
  int num_obj_info;
  char **obj_info;

  /* open a PLY file for reading */
  ply = ply_open_for_reading(filename, &nelems, &elist, &file_type, &version);

#ifdef DEBUG
  /* print what we found out about the file */
  printf ("version %f\n", version);
  printf ("type %d\n", file_type);
#endif

  /* go through each kind of element that we learned is in the file */
  /* and read them */

  for (i = 0; i < nelems; i++) {

    /* get the description of the first element */
    elem_name = elist[i];
    plist = ply_get_element_description (ply, elem_name, &num_elems, &nprops);

#ifdef DEBUG
    /* print the name of the element, for debugging */
    printf ("element %s %d\n", elem_name, num_elems);
#endif

    /* if we're on vertex elements, read them in */
    if (equal_strings ("vertex", elem_name)) {

      /* create a vertex list to hold all the vertices */
      vlist = (Vertex **) malloc (sizeof (Vertex *) * num_elems);

      /* set up for getting vertex elements */

      ply_get_property (ply, elem_name, &vert_props[0]);
      ply_get_property (ply, elem_name, &vert_props[1]);
      ply_get_property (ply, elem_name, &vert_props[2]);
      ply_get_property (ply, elem_name, &vert_props[3]);
      ply_get_property (ply, elem_name, &vert_props[4]);
      ply_get_property (ply, elem_name, &vert_props[5]);

      /* grab all the vertex elements */
      for (j = 0; j < num_elems; j++) {

        /* grab and element from the file */
        vlist[j] = (Vertex *) malloc (sizeof (Vertex));
        ply_get_element (ply, (void *) vlist[j]);

#ifdef DEBUG
        /* print out vertex x,y,z for debugging */
        printf ("vertex: %g %g %g\n", vlist[j]->x, vlist[j]->y, vlist[j]->z);
#endif
      }
    }

    /* if we're on face elements, read them in */
    if (equal_strings ("face", elem_name)) {

      /* create a list to hold all the face elements */
      flist = (Face **) malloc (sizeof (Face *) * num_elems);

      /* set up for getting face elements */

      ply_get_property (ply, elem_name, &face_props[0]);
      ply_get_property (ply, elem_name, &face_props[1]);

      /* grab all the face elements */
      for (j = 0; j < num_elems; j++) {

        /* grab and element from the file */
        flist[j] = (Face *) malloc (sizeof (Face));
        ply_get_element (ply, (void *) flist[j]);

#ifdef DEBUG
        /* print out face info, for debugging */
        printf ("face: %d, list = ", flist[j]->intensity);
        for (k = 0; k < flist[j]->nverts; k++)
          printf ("%d ", flist[j]->verts[k]);
        printf ("\n");
#endif
      }
    }

#ifdef DEBUG
    /* print out the properties we got, for debugging */
    for (j = 0; j < nprops; j++)
      printf ("property %s\n", plist[j]->name);
#endif
  }

  /* grab and print out the comments in the file */
  comments = ply_get_comments (ply, &num_comments);
#ifdef DEBUG
  for (i = 0; i < num_comments; i++)
    printf ("comment = '%s'\n", comments[i]);
#endif

  /* grab and print out the object information */
  obj_info = ply_get_obj_info (ply, &num_obj_info);
#ifdef DEBUG
  for (i = 0; i < num_obj_info; i++)
    printf ("obj_info = '%s'\n", obj_info[i]);
#endif

  /* close the PLY file */
  ply_close (ply);
}
