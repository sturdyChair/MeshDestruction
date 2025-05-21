#pragma once
#include "hwiVoro.h"

#include <cstdio>
#include <vector>

#include "hwiVoro_Defines.h"
#include "commons.h"
#include "HV_Base.h"
#include "HV_Cell.h"
#include "c_loop.h"
#include "HV_Compute.h"
#include "rad_option.h"
#include "unitcell.h"

namespace hwiVoro {

	/** \brief Pure virtual class from which wall objects are derived.
	 *
	 * This is a pure virtual class for a generic wall object. A wall object
	 * can be specified by deriving a new class from this and specifying the
	 * functions.*/
	class HWIVORO_DLL wall {
	public:
		virtual ~wall() {}
		/** A pure virtual function for testing whether a point is
		 * inside the wall object. */
		virtual bool point_inside(double x, double y, double z) = 0;
		/** A pure virtual function for cutting a cell without
		 * neighbor-tracking with a wall. */
		virtual bool cut_cell(CHV_Cell& c, double x, double y, double z) = 0;
		/** A pure virtual function for cutting a cell with
		 * neighbor-tracking enabled with a wall. */
		virtual bool cut_cell(CHV_Cell_Neighbor& c, double x, double y, double z) = 0;
	};

	/** \brief A class for storing a list of pointers to walls.
	 *
	 * This class stores a list of pointers to wall classes. It contains several
	 * simple routines that make use of the wall classes (such as telling whether a
	 * given position is inside all of the walls or not). It can be used by itself,
	 * but also forms part of container_base, for associating walls with this
	 * class. */
	class HWIVORO_DLL wall_list {
	public:
		/** An array holding pointers to wall objects. */
		wall** walls;
		/** A pointer to the next free position to add a wall pointer.
		 */
		wall** wep;
		wall_list();
		~wall_list();
		/** Adds a wall to the list.
		 * \param[in] w the wall to add. */
		inline void add_wall(wall* w) {
			if (wep == wel) increase_wall_memory();
			*(wep++) = w;
		}
		/** Adds a wall to the list.
		 * \param[in] w a reference to the wall to add. */
		inline void add_wall(wall& w) { add_wall(&w); }
		void add_wall(wall_list& wl);
		/** Determines whether a given position is inside all of the
		 * walls on the list.
		 * \param[in] (x,y,z) the position to test.
		 * \return True if it is inside, false if it is outside. */
		inline bool point_inside_walls(double x, double y, double z) {
			for (wall** wp = walls; wp < wep; wp++) if (!((*wp)->point_inside(x, y, z))) return false;
			return true;
		}
		/** Cuts a Voronoi cell by all of the walls currently on
		 * the list.
		 * \param[in] c a reference to the Voronoi cell class.
		 * \param[in] (x,y,z) the position of the cell.
		 * \return True if the cell still exists, false if the cell is
		 * deleted. */
		template<class c_class>
		bool apply_walls(c_class& c, double x, double y, double z) {
			for (wall** wp = walls; wp < wep; wp++) if (!((*wp)->cut_cell(c, x, y, z))) return false;
			return true;
		}
		void deallocate();
	protected:
		void increase_wall_memory();
		/** A pointer to the limit of the walls array, used to
		 * determine when array is full. */
		wall** wel;
		/** The current amount of memory allocated for walls. */
		int current_wall_size;
	};

	/** \brief Class for representing a particle system in a three-dimensional
	 * rectangular box.
	 *
	 * This class represents a system of particles in a three-dimensional
	 * rectangular box. Any combination of non-periodic and periodic coordinates
	 * can be used in the three coordinate directions. The class is not intended
	 * for direct use, but instead forms the base of the container and
	 * container_poly classes that add specialized routines for computing the
	 * regular and radical Voronoi tessellations respectively. It contains routines
	 * that are commonly between these two classes, such as those for drawing the
	 * domain, and placing particles within the internal data structure.
	 *
	 * The class is derived from the wall_list class, which encapsulates routines
	 * for associating walls with the container, and the CHV_Base class, which
	 * encapsulates routines about the underlying computational grid. */
	class HWIVORO_DLL container_base : public CHV_Base, public wall_list {
	public:
		/** The minimum x coordinate of the container. */
		const double ax;
		/** The maximum x coordinate of the container. */
		const double bx;
		/** The minimum y coordinate of the container. */
		const double ay;
		/** The maximum y coordinate of the container. */
		const double by;
		/** The minimum z coordinate of the container. */
		const double az;
		/** The maximum z coordinate of the container. */
		const double bz;
		/** A boolean value that determines if the x coordinate in
		 * periodic or not. */
		const bool xperiodic;
		/** A boolean value that determines if the y coordinate in
		 * periodic or not. */
		const bool yperiodic;
		/** A boolean value that determines if the z coordinate in
		 * periodic or not. */
		const bool zperiodic;
		/** This array holds the numerical IDs of each particle in each
		 * computational box. */
		int** id;
		/** A two dimensional array holding particle positions. For the
		 * derived container_poly class, this also holds particle
		 * radii. */
		double** p;
		/** This array holds the number of particles within each
		 * computational box of the container. */
		int* co;
		/** This array holds the maximum amount of particle memory for
		 * each computational box of the container. If the number of
		 * particles in a particular box ever approaches this limit,
		 * more is allocated using the add_particle_memory() function.
		 */
		int* mem;
		/** The amount of memory in the array structure for each
		 * particle. This is set to 3 when the basic class is
		 * initialized, so that the array holds (x,y,z) positions. If
		 * the container class is initialized as part of the derived
		 * class container_poly, then this is set to 4, to also hold
		 * the particle radii. */
		const int ps;
		container_base(double ax_, double bx_, double ay_, double by_, double az_, double bz_,
			int nx_, int ny_, int nz_, bool xperiodic_, bool yperiodic_, bool zperiodic_,
			int init_mem, int ps_);
		~container_base();
		bool point_inside(double x, double y, double z);
		void region_count();
		/** Initializes the Voronoi cell prior to a compute_cell
		 * operation for a specific particle being carried out by a
		 * CHV_Compute class. The cell is initialized to fill the
		 * entire container. For non-periodic coordinates, this is set
		 * by the position of the walls. For periodic coordinates, the
		 * space is equally divided in either direction from the
		 * particle's initial position. Plane cuts made by any walls
		 * that have been added are then applied to the cell.
		 * \param[in,out] c a reference to a CHV_Cell object.
		 * \param[in] ijk the block that the particle is within.
		 * \param[in] q the index of the particle within its block.
		 * \param[in] (ci,cj,ck) the coordinates of the block in the
		 * 			 container coordinate system.
		 * \param[out] (i,j,k) the coordinates of the test block
		 * 		       relative to the CHV_Compute
		 * 		       coordinate system.
		 * \param[out] (x,y,z) the position of the particle.
		 * \param[out] disp a block displacement used internally by the
		 *		    compute_cell routine.
		 * \return False if the plane cuts applied by walls completely
		 * removed the cell, true otherwise. */
		template<class v_cell>
		inline bool initialize_CHV_Cell(v_cell& c, int ijk, int q, int ci, int cj, int ck,
			int& i, int& j, int& k, double& x, double& y, double& z, int& disp) {
			double x1, x2, y1, y2, z1, z2, * pp = p[ijk] + ps * q;
			x = *(pp++); y = *(pp++); z = *pp;
			if (xperiodic) { x1 = -(x2 = 0.5 * (bx - ax)); i = nx; }
			else { x1 = ax - x; x2 = bx - x; i = ci; }
			if (yperiodic) { y1 = -(y2 = 0.5 * (by - ay)); j = ny; }
			else { y1 = ay - y; y2 = by - y; j = cj; }
			if (zperiodic) { z1 = -(z2 = 0.5 * (bz - az)); k = nz; }
			else { z1 = az - z; z2 = bz - z; k = ck; }
			c.init(x1, x2, y1, y2, z1, z2);
			if (!apply_walls(c, x, y, z)) return false;
			disp = ijk - i - nx * (j + ny * k);
			return true;
		}
		/** Initializes parameters for a find_voronoi_cell call within
		 * the CHV_Compute template.
		 * \param[in] (ci,cj,ck) the coordinates of the test block in
		 * 			 the container coordinate system.
		 * \param[in] ijk the index of the test block
		 * \param[out] (i,j,k) the coordinates of the test block
		 * 		       relative to the CHV_Compute
		 * 		       coordinate system.
		 * \param[out] disp a block displacement used internally by the
		 *		    find_voronoi_cell routine. */
		inline void initialize_search(int ci, int cj, int ck, int ijk, int& i, int& j, int& k, int& disp) {
			i = xperiodic ? nx : ci;
			j = yperiodic ? ny : cj;
			k = zperiodic ? nz : ck;
			disp = ijk - i - nx * (j + ny * k);
		}
		/** Returns the position of a particle currently being computed
		 * relative to the computational block that it is within. It is
		 * used to select the optimal worklist entry to use.
		 * \param[in] (x,y,z) the position of the particle.
		 * \param[in] (ci,cj,ck) the block that the particle is within.
		 * \param[out] (fx,fy,fz) the position relative to the block.
		 */
		inline void frac_pos(double x, double y, double z, double ci, double cj, double ck,
			double& fx, double& fy, double& fz) {
			fx = x - ax - boxx * ci;
			fy = y - ay - boxy * cj;
			fz = z - az - boxz * ck;
		}
		/** Calculates the index of block in the container structure
		 * corresponding to given coordinates.
		 * \param[in] (ci,cj,ck) the coordinates of the original block
		 * 			 in the current computation, relative
		 * 			 to the container coordinate system.
		 * \param[in] (ei,ej,ek) the displacement of the current block
		 * 			 from the original block.
		 * \param[in,out] (qx,qy,qz) the periodic displacement that
		 * 			     must be added to the particles
		 * 			     within the computed block.
		 * \param[in] disp a block displacement used internally by the
		 * 		    find_voronoi_cell and compute_cell routines.
		 * \return The block index. */
		inline int region_index(int ci, int cj, int ck, int ei, int ej, int ek, double& qx, double& qy, double& qz, int& disp) {
			if (xperiodic) { if (ci + ei < nx) { ei += nx; qx = -(bx - ax); } else if (ci + ei >= (nx << 1)) { ei -= nx; qx = bx - ax; } else qx = 0; }
			if (yperiodic) { if (cj + ej < ny) { ej += ny; qy = -(by - ay); } else if (cj + ej >= (ny << 1)) { ej -= ny; qy = by - ay; } else qy = 0; }
			if (zperiodic) { if (ck + ek < nz) { ek += nz; qz = -(bz - az); } else if (ck + ek >= (nz << 1)) { ek -= nz; qz = bz - az; } else qz = 0; }
			return disp + ei + nx * (ej + ny * ek);
		}
		void draw_domain_gnuplot(FILE* fp = stdout);
		/** Draws an outline of the domain in Gnuplot format.
		 * \param[in] filename the filename to write to. */
		inline void draw_domain_gnuplot(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_domain_gnuplot(fp);
			fclose(fp);
		}
		void draw_domain_pov(FILE* fp = stdout);
		/** Draws an outline of the domain in Gnuplot format.
		 * \param[in] filename the filename to write to. */
		inline void draw_domain_pov(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_domain_pov(fp);
			fclose(fp);
		}
		/** Sums up the total number of stored particles.
		 * \return The number of particles. */
		inline int total_particles() {
			int tp = *co;
			for (int* cop = co + 1; cop < co + nxyz; cop++) tp += *cop;
			return tp;
		}
	protected:
		void add_particle_memory(int i);
		bool put_locate_block(int& ijk, double& x, double& y, double& z);
		inline bool put_remap(int& ijk, double& x, double& y, double& z);
		inline bool remap(int& ai, int& aj, int& ak, int& ci, int& cj, int& ck, double& x, double& y, double& z, int& ijk);
	};

	/** \brief Extension of the container_base class for computing regular Voronoi
	 * tessellations.
	 *
	 * This class is an extension of the container_base class that has routines
	 * specifically for computing the regular Voronoi tessellation with no
	 * dependence on particle radii. */
	class HWIVORO_DLL container : public container_base, public radius_mono {
	public:
		container(double ax_, double bx_, double ay_, double by_, double az_, double bz_,
			int nx_, int ny_, int nz_, bool xperiodic_, bool yperiodic_, bool zperiodic_, int init_mem);
		void clear();
		void put(int n, double x, double y, double z);
		void put(particle_order& vo, int n, double x, double y, double z);
		void import(FILE * fp = stdin);
		void import(particle_order & vo, FILE * fp = stdin);
		/** Imports a list of particles from an open file stream into
		 * the container. Entries of four numbers (Particle ID, x
		 * position, y position, z position) are searched for. If the
		 * file cannot be successfully read, then the routine causes a
		 * fatal error.
		 * \param[in] filename the name of the file to open and read
		 *                     from. */
		inline void import(const char* filename) {
			FILE* fp = safe_fopen(filename, "r");
import(fp);
			fclose(fp);
		}
		/** Imports a list of particles from an open file stream into
		 * the container. Entries of four numbers (Particle ID, x
		 * position, y position, z position) are searched for. In
		 * addition, the order in which particles are read is saved
		 * into an ordering class. If the file cannot be successfully
		 * read, then the routine causes a fatal error.
		 * \param[in,out] vo the ordering class to use.
		 * \param[in] filename the name of the file to open and read
		 *                     from. */
		inline void import(particle_order & vo, const char* filename) {
			FILE* fp = safe_fopen(filename, "r");
import(vo,fp);
			fclose(fp);
		}
		void compute_all_cells();
		double sum_cell_volumes();
		/** Dumps particle IDs and positions to a file.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_particles(c_loop& vl, FILE* fp) {
			double* pp;
			if (vl.start()) do {
				pp = p[vl.ijk] + 3 * vl.q;
				fprintf(fp, "%d %g %g %g\n", id[vl.ijk][vl.q], *pp, pp[1], pp[2]);
			} while (vl.inc());
		}
		/** Dumps all of the particle IDs and positions to a file.
		 * \param[in] fp a file handle to write to. */
		inline void draw_particles(FILE* fp = stdout) {
			c_loop_all vl(*this);
			draw_particles(vl, fp);
		}
		/** Dumps all of the particle IDs and positions to a file.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_particles(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_particles(fp);
			fclose(fp);
		}
		/** Dumps particle positions in POV-Ray format.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_particles_pov(c_loop& vl, FILE* fp) {
			double* pp;
			if (vl.start()) do {
				pp = p[vl.ijk] + 3 * vl.q;
				fprintf(fp, "// id %d\nsphere{<%g,%g,%g>,s}\n",
					id[vl.ijk][vl.q], *pp, pp[1], pp[2]);
			} while (vl.inc());
		}
		/** Dumps all particle positions in POV-Ray format.
		 * \param[in] fp a file handle to write to. */
		inline void draw_particles_pov(FILE* fp = stdout) {
			c_loop_all vl(*this);
			draw_particles_pov(vl, fp);
		}
		/** Dumps all particle positions in POV-Ray format.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_particles_pov(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_particles_pov(fp);
			fclose(fp);
		}
		/** Computes Voronoi cells and saves the output in gnuplot
		 * format.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_cells_gnuplot(c_loop& vl, FILE* fp) {
			CHV_Cell c; double* pp;
			if (vl.start()) do if (compute_cell(c, vl)) {
				pp = p[vl.ijk] + ps * vl.q;
				c.draw_gnuplot(*pp, pp[1], pp[2], fp);
			} while (vl.inc());
		}
		/** Computes all Voronoi cells and saves the output in gnuplot
		 * format.
		 * \param[in] fp a file handle to write to. */
		inline void draw_cells_gnuplot(FILE* fp = stdout) {
			c_loop_all vl(*this);
			draw_cells_gnuplot(vl, fp);
		}
		/** Computes all Voronoi cells and saves the output in gnuplot
		 * format.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_cells_gnuplot(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_cells_gnuplot(fp);
			fclose(fp);
		}
		/** Computes Voronoi cells and saves the output in POV-Ray
		 * format.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_cells_pov(c_loop& vl, FILE* fp) {
			CHV_Cell c; double* pp;
			if (vl.start()) do if (compute_cell(c, vl)) {
				fprintf(fp, "// cell %d\n", id[vl.ijk][vl.q]);
				pp = p[vl.ijk] + ps * vl.q;
				c.draw_pov(*pp, pp[1], pp[2], fp);
			} while (vl.inc());
		}
		/** Computes all Voronoi cells and saves the output in POV-Ray
		 * format.
		 * \param[in] fp a file handle to write to. */
		inline void draw_cells_pov(FILE* fp = stdout) {
			c_loop_all vl(*this);
			draw_cells_pov(vl, fp);
		}
		/** Computes all Voronoi cells and saves the output in POV-Ray
		 * format.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_cells_pov(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_cells_pov(fp);
			fclose(fp);
		}
		/** Computes the Voronoi cells and saves customized information
		 * about them.
		 * \param[in] vl the loop class to use.
		 * \param[in] format the custom output string to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void print_custom(c_loop& vl, const char* format, FILE* fp) {
			int ijk, q; double* pp;
			if (contains_neighbor(format)) {
				CHV_Cell_Neighbor c;
				if (vl.start()) do if (compute_cell(c, vl)) {
					ijk = vl.ijk; q = vl.q; pp = p[ijk] + ps * q;
					c.output_custom(format, id[ijk][q], *pp, pp[1], pp[2], default_radius, fp);
				} while (vl.inc());
			}
			else {
				CHV_Cell c;
				if (vl.start()) do if (compute_cell(c, vl)) {
					ijk = vl.ijk; q = vl.q; pp = p[ijk] + ps * q;
					c.output_custom(format, id[ijk][q], *pp, pp[1], pp[2], default_radius, fp);
				} while (vl.inc());
			}
		}
		void print_custom(const char* format, FILE* fp = stdout);
		void print_custom(const char* format, const char* filename);
		bool find_voronoi_cell(double x, double y, double z, double& rx, double& ry, double& rz, int& pid);
		/** Computes the Voronoi cell for a particle currently being
		 * referenced by a loop class.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] vl the loop class to use.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed, if it is removed entirely by a wall or boundary
		 * condition, then the routine returns false. */
		template<class v_cell, class c_loop>
		inline bool compute_cell(v_cell& c, c_loop& vl) {
			return vc.compute_cell(c, vl.ijk, vl.q, vl.i, vl.j, vl.k);
		}
		/** Computes the Voronoi cell for given particle.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] ijk the block that the particle is within.
		 * \param[in] q the index of the particle within the block.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed, if it is removed entirely by a wall or boundary
		 * condition, then the routine returns false. */
		template<class v_cell>
		inline bool compute_cell(v_cell& c, int ijk, int q) {
			int k = ijk / nxy, ijkt = ijk - nxy * k, j = ijkt / nx, i = ijkt - j * nx;
			return vc.compute_cell(c, ijk, q, i, j, k);
		}
		/** Computes the Voronoi cell for a ghost particle at a given
		 * location.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] (x,y,z) the location of the ghost particle.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed, if it is removed entirely by a wall or boundary
		 * condition, then the routine returns false. */
		template<class v_cell>
		inline bool compute_ghost_cell(v_cell& c, double x, double y, double z) {
			int ijk;
			if (put_locate_block(ijk, x, y, z)) {
				double* pp = p[ijk] + 3 * co[ijk]++;
				*(pp++) = x; *(pp++) = y; *pp = z;
				bool q = compute_cell(c, ijk, co[ijk] - 1);
				co[ijk]--;
				return q;
			}
			return false;
		}
	private:
		CHV_Compute<container> vc;
		friend class CHV_Compute<container>;
	};

	/** \brief Extension of the container_base class for computing radical Voronoi
	 * tessellations.
	 *
	 * This class is an extension of container_base class that has routines
	 * specifically for computing the radical Voronoi tessellation that depends on
	 * the particle radii. */
	class HWIVORO_DLL container_poly : public container_base, public radius_poly {
	public:
		container_poly(double ax_, double bx_, double ay_, double by_, double az_, double bz_,
			int nx_, int ny_, int nz_, bool xperiodic_, bool yperiodic_, bool zperiodic_, int init_mem);
		void clear();
		void put(int n, double x, double y, double z, double r);
		void put(particle_order& vo, int n, double x, double y, double z, double r);
		void import(FILE * fp = stdin);
		void import(particle_order & vo, FILE * fp = stdin);
		/** Imports a list of particles from an open file stream into
		 * the container_poly class. Entries of five numbers (Particle
		 * ID, x position, y position, z position, radius) are searched
		 * for. If the file cannot be successfully read, then the
		 * routine causes a fatal error.
		 * \param[in] filename the name of the file to open and read
		 *                     from. */
		inline void import(const char* filename) {
			FILE* fp = safe_fopen(filename, "r");
import(fp);
			fclose(fp);
		}
		/** Imports a list of particles from an open file stream into
		 * the container_poly class. Entries of five numbers (Particle
		 * ID, x position, y position, z position, radius) are searched
		 * for. In addition, the order in which particles are read is
		 * saved into an ordering class. If the file cannot be
		 * successfully read, then the routine causes a fatal error.
		 * \param[in,out] vo the ordering class to use.
		 * \param[in] filename the name of the file to open and read
		 *                     from. */
		inline void import(particle_order & vo, const char* filename) {
			FILE* fp = safe_fopen(filename, "r");
import(vo,fp);
			fclose(fp);
		}
		void compute_all_cells();
		double sum_cell_volumes();
		/** Dumps particle IDs, positions and radii to a file.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_particles(c_loop& vl, FILE* fp) {
			double* pp;
			if (vl.start()) do {
				pp = p[vl.ijk] + 4 * vl.q;
				fprintf(fp, "%d %g %g %g %g\n", id[vl.ijk][vl.q], *pp, pp[1], pp[2], pp[3]);
			} while (vl.inc());
		}
		/** Dumps all of the particle IDs, positions and radii to a
		 * file.
		 * \param[in] fp a file handle to write to. */
		inline void draw_particles(FILE* fp = stdout) {
			c_loop_all vl(*this);
			draw_particles(vl, fp);
		}
		/** Dumps all of the particle IDs, positions and radii to a
		 * file.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_particles(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_particles(fp);
			fclose(fp);
		}
		/** Dumps particle positions in POV-Ray format.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_particles_pov(c_loop& vl, FILE* fp) {
			double* pp;
			if (vl.start()) do {
				pp = p[vl.ijk] + 4 * vl.q;
				fprintf(fp, "// id %d\nsphere{<%g,%g,%g>,%g}\n",
					id[vl.ijk][vl.q], *pp, pp[1], pp[2], pp[3]);
			} while (vl.inc());
		}
		/** Dumps all the particle positions in POV-Ray format.
		 * \param[in] fp a file handle to write to. */
		inline void draw_particles_pov(FILE* fp = stdout) {
			c_loop_all vl(*this);
			draw_particles_pov(vl, fp);
		}
		/** Dumps all the particle positions in POV-Ray format.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_particles_pov(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_particles_pov(fp);
			fclose(fp);
		}
		/** Computes Voronoi cells and saves the output in gnuplot
		 * format.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_cells_gnuplot(c_loop& vl, FILE* fp) {
			CHV_Cell c; double* pp;
			if (vl.start()) do if (compute_cell(c, vl)) {
				pp = p[vl.ijk] + ps * vl.q;
				c.draw_gnuplot(*pp, pp[1], pp[2], fp);
			} while (vl.inc());
		}
		/** Compute all Voronoi cells and saves the output in gnuplot
		 * format.
		 * \param[in] fp a file handle to write to. */
		inline void draw_cells_gnuplot(FILE* fp = stdout) {
			c_loop_all vl(*this);
			draw_cells_gnuplot(vl, fp);
		}
		/** Compute all Voronoi cells and saves the output in gnuplot
		 * format.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_cells_gnuplot(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_cells_gnuplot(fp);
			fclose(fp);
		}
		/** Computes Voronoi cells and saves the output in POV-Ray
		 * format.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_cells_pov(c_loop& vl, FILE* fp) {
			CHV_Cell c; double* pp;
			if (vl.start()) do if (compute_cell(c, vl)) {
				fprintf(fp, "// cell %d\n", id[vl.ijk][vl.q]);
				pp = p[vl.ijk] + ps * vl.q;
				c.draw_pov(*pp, pp[1], pp[2], fp);
			} while (vl.inc());
		}
		/** Computes all Voronoi cells and saves the output in POV-Ray
		 * format.
		 * \param[in] fp a file handle to write to. */
		inline void draw_cells_pov(FILE* fp = stdout) {
			c_loop_all vl(*this);
			draw_cells_pov(vl, fp);
		}
		/** Computes all Voronoi cells and saves the output in POV-Ray
		 * format.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_cells_pov(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_cells_pov(fp);
			fclose(fp);
		}
		/** Computes the Voronoi cells and saves customized information
		 * about them.
		 * \param[in] vl the loop class to use.
		 * \param[in] format the custom output string to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void print_custom(c_loop& vl, const char* format, FILE* fp) {
			int ijk, q; double* pp;
			if (contains_neighbor(format)) {
				CHV_Cell_Neighbor c;
				if (vl.start()) do if (compute_cell(c, vl)) {
					ijk = vl.ijk; q = vl.q; pp = p[ijk] + ps * q;
					c.output_custom(format, id[ijk][q], *pp, pp[1], pp[2], pp[3], fp);
				} while (vl.inc());
			}
			else {
				CHV_Cell c;
				if (vl.start()) do if (compute_cell(c, vl)) {
					ijk = vl.ijk; q = vl.q; pp = p[ijk] + ps * q;
					c.output_custom(format, id[ijk][q], *pp, pp[1], pp[2], pp[3], fp);
				} while (vl.inc());
			}
		}
		/** Computes the Voronoi cell for a particle currently being
		 * referenced by a loop class.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] vl the loop class to use.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed, if it is removed entirely by a wall or boundary
		 * condition, then the routine returns false. */
		template<class v_cell, class c_loop>
		inline bool compute_cell(v_cell& c, c_loop& vl) {
			return vc.compute_cell(c, vl.ijk, vl.q, vl.i, vl.j, vl.k);
		}
		/** Computes the Voronoi cell for given particle.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] ijk the block that the particle is within.
		 * \param[in] q the index of the particle within the block.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed, if it is removed entirely by a wall or boundary
		 * condition, then the routine returns false. */
		template<class v_cell>
		inline bool compute_cell(v_cell& c, int ijk, int q) {
			int k = ijk / nxy, ijkt = ijk - nxy * k, j = ijkt / nx, i = ijkt - j * nx;
			return vc.compute_cell(c, ijk, q, i, j, k);
		}
		/** Computes the Voronoi cell for a ghost particle at a given
		 * location.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] (x,y,z) the location of the ghost particle.
		 * \param[in] r the radius of the ghost particle.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed, if it is removed entirely by a wall or boundary
		 * condition, then the routine returns false. */
		template<class v_cell>
		inline bool compute_ghost_cell(v_cell& c, double x, double y, double z, double r) {
			int ijk;
			if (put_locate_block(ijk, x, y, z)) {
				double* pp = p[ijk] + 4 * co[ijk]++, tm = max_radius;
				*(pp++) = x; *(pp++) = y; *(pp++) = z; *pp = r;
				if (r > max_radius) max_radius = r;
				bool q = compute_cell(c, ijk, co[ijk] - 1);
				co[ijk]--; max_radius = tm;
				return q;
			}
			return false;
		}
		void print_custom(const char* format, FILE* fp = stdout);
		void print_custom(const char* format, const char* filename);
		bool find_voronoi_cell(double x, double y, double z, double& rx, double& ry, double& rz, int& pid);
	private:
		CHV_Compute<container_poly> vc;
		friend class CHV_Compute<container_poly>;
	};


	class HWIVORO_DLL container_periodic_base : public unitcell, public CHV_Base {
	public:
		/** The lower y index (inclusive) of the primary domain within
		 * the block structure. */
		int ey;
		/** The lower z index (inclusive) of the primary domain within
		 * the block structure. */
		int ez;
		/** The upper y index (exclusive) of the primary domain within
		 * the block structure. */
		int wy;
		/** The upper z index (exclusive) of the primary domain within
		 * the block structure. */
		int wz;
		/** The total size of the block structure (including images) in
		 * the y direction. */
		int oy;
		/** The total size of the block structure (including images) in
		 * the z direction. */
		int oz;
		/** The total number of blocks. */
		int oxyz;
		/** This array holds the numerical IDs of each particle in each
		 * computational box. */
		int** id;
		/** A two dimensional array holding particle positions. For the
		 * derived container_poly class, this also holds particle
		 * radii. */
		double** p;
		/** This array holds the number of particles within each
		 * computational box of the container. */
		int* co;
		/** This array holds the maximum amount of particle memory for
		 * each computational box of the container. If the number of
		 * particles in a particular box ever approaches this limit,
		 * more is allocated using the add_particle_memory() function.
		 */
		int* mem;
		/** An array holding information about periodic image
		 * construction at a given location. */
		char* img;
		/** The initial amount of memory to allocate for particles
		 * for each block. */
		const int init_mem;
		/** The amount of memory in the array structure for each
		 * particle. This is set to 3 when the basic class is
		 * initialized, so that the array holds (x,y,z) positions. If
		 * the container class is initialized as part of the derived
		 * class container_poly, then this is set to 4, to also hold
		 * the particle radii. */
		const int ps;
		container_periodic_base(double bx_, double bxy_, double by_, double bxz_, double byz_, double bz_,
			int nx_, int ny_, int nz_, int init_mem_, int ps);
		~container_periodic_base();
		/** Prints all particles in the container, including those that
		 * have been constructed in image blocks. */
		inline void print_all_particles() {
			int ijk, q;
			for (ijk = 0; ijk < oxyz; ijk++) for (q = 0; q < co[ijk]; q++)
				printf("%d %g %g %g\n", id[ijk][q], p[ijk][ps * q], p[ijk][ps * q + 1], p[ijk][ps * q + 2]);
		}
		void region_count();
		/** Initializes the Voronoi cell prior to a compute_cell
		 * operation for a specific particle being carried out by a
		 * CHV_Compute class. The cell is initialized to be the
		 * pre-computed unit Voronoi cell based on planes formed by
		 * periodic images of the particle.
		 * \param[in,out] c a reference to a CHV_Cell object.
		 * \param[in] ijk the block that the particle is within.
		 * \param[in] q the index of the particle within its block.
		 * \param[in] (ci,cj,ck) the coordinates of the block in the
		 * 			 container coordinate system.
		 * \param[out] (i,j,k) the coordinates of the test block
		 * 		       relative to the CHV_Compute
		 * 		       coordinate system.
		 * \param[out] (x,y,z) the position of the particle.
		 * \param[out] disp a block displacement used internally by the
		 *		    compute_cell routine.
		 * \return False if the plane cuts applied by walls completely
		 * removed the cell, true otherwise. */
		template<class v_cell>
		inline bool initialize_CHV_Cell(v_cell& c, int ijk, int q, int ci, int cj, int ck, int& i, int& j, int& k, double& x, double& y, double& z, int& disp) {
			c = unit_voro;
			double* pp = p[ijk] + ps * q;
			x = *(pp++); y = *(pp++); z = *pp;
			i = nx; j = ey; k = ez;
			return true;
		}
		/** Initializes parameters for a find_voronoi_cell call within
		 * the CHV_Compute template.
		 * \param[in] (ci,cj,ck) the coordinates of the test block in
		 * 			 the container coordinate system.
		 * \param[in] ijk the index of the test block
		 * \param[out] (i,j,k) the coordinates of the test block
		 * 		       relative to the CHV_Compute
		 * 		       coordinate system.
		 * \param[out] disp a block displacement used internally by the
		 *		    find_voronoi_cell routine (but not needed
		 *		    in this instance.) */
		inline void initialize_search(int ci, int cj, int ck, int ijk, int& i, int& j, int& k, int& disp) {
			i = nx; j = ey; k = ez;
		}
		/** Returns the position of a particle currently being computed
		 * relative to the computational block that it is within. It is
		 * used to select the optimal worklist entry to use.
		 * \param[in] (x,y,z) the position of the particle.
		 * \param[in] (ci,cj,ck) the block that the particle is within.
		 * \param[out] (fx,fy,fz) the position relative to the block.
		 */
		inline void frac_pos(double x, double y, double z, double ci, double cj, double ck, double& fx, double& fy, double& fz) {
			fx = x - boxx * ci;
			fy = y - boxy * (cj - ey);
			fz = z - boxz * (ck - ez);
		}
		/** Calculates the index of block in the container structure
		 * corresponding to given coordinates.
		 * \param[in] (ci,cj,ck) the coordinates of the original block
		 * 			 in the current computation, relative
		 * 			 to the container coordinate system.
		 * \param[in] (ei,ej,ek) the displacement of the current block
		 * 			 from the original block.
		 * \param[in,out] (qx,qy,qz) the periodic displacement that
		 * 			     must be added to the particles
		 * 			     within the computed block.
		 * \param[in] disp a block displacement used internally by the
		 * 		    find_voronoi_cell and compute_cell routines
		 * 		    (but not needed in this instance.)
		 * \return The block index. */
		inline int region_index(int ci, int cj, int ck, int ei, int ej, int ek, double& qx, double& qy, double& qz, int& disp) {
			int qi = ci + (ei - nx), qj = cj + (ej - ey), qk = ck + (ek - ez);
			int iv(step_div(qi, nx)); if (iv != 0) { qx = iv * bx; qi -= nx * iv; }
			else qx = 0;
			create_periodic_image(qi, qj, qk);
			return qi + nx * (qj + oy * qk);
		}
		void create_all_images();
		void check_compartmentalized();
	protected:
		void add_particle_memory(int i);
		void put_locate_block(int& ijk, double& x, double& y, double& z);
		void put_locate_block(int& ijk, double& x, double& y, double& z, int& ai, int& aj, int& ak);
		/** Creates particles within an image block by copying them
		 * from the primary domain and shifting them. If the given
		 * block is aligned with the primary domain in the z-direction,
		 * the routine calls the simpler create_side_image routine
		 * where the image block may comprise of particles from up to
		 * two primary blocks. Otherwise is calls the more complex
		 * create_vertical_image where the image block may comprise of
		 * particles from up to four primary blocks.
		 * \param[in] (di,dj,dk) the coordinates of the image block to
		 *                       create. */
		inline void create_periodic_image(int di, int dj, int dk) {
			if (di < 0 || di >= nx || dj < 0 || dj >= oy || dk < 0 || dk >= oz)
				voro_fatal_error("Constructing periodic image for nonexistent point", VOROPP_INTERNAL_ERROR);
			if (dk >= ez && dk < wz) {
				if (dj < ey || dj >= wy) create_side_image(di, dj, dk);
			}
			else create_vertical_image(di, dj, dk);
		}
		void create_side_image(int di, int dj, int dk);
		void create_vertical_image(int di, int dj, int dk);
		void put_image(int reg, int fijk, int l, double dx, double dy, double dz);
		inline void remap(int& ai, int& aj, int& ak, int& ci, int& cj, int& ck, double& x, double& y, double& z, int& ijk);
	};

	/** \brief Extension of the container_periodic_base class for computing regular
	 * Voronoi tessellations.
	 *
	 * This class is an extension of the container_periodic_base that has routines
	 * specifically for computing the regular Voronoi tessellation with no
	 * dependence on particle radii. */
	class HWIVORO_DLL container_periodic : public container_periodic_base, public radius_mono {
	public:
		container_periodic(double bx_, double bxy_, double by_, double bxz_, double byz_, double bz_,
			int nx_, int ny_, int nz_, int init_mem_);
		void clear();
		void put(int n, double x, double y, double z);
		void put(int n, double x, double y, double z, int& ai, int& aj, int& ak);
		void put(particle_order& vo, int n, double x, double y, double z);
		void import(FILE * fp = stdin);
		void import(particle_order & vo, FILE * fp = stdin);
		/** Imports a list of particles from an open file stream into
		 * the container. Entries of four numbers (Particle ID, x
		 * position, y position, z position) are searched for. If the
		 * file cannot be successfully read, then the routine causes a
		 * fatal error.
		 * \param[in] filename the name of the file to open and read
		 *                     from. */
		inline void import(const char* filename) {
			FILE* fp = safe_fopen(filename, "r");
import(fp);
			fclose(fp);
		}
		/** Imports a list of particles from an open file stream into
		 * the container. Entries of four numbers (Particle ID, x
		 * position, y position, z position) are searched for. In
		 * addition, the order in which particles are read is saved
		 * into an ordering class. If the file cannot be successfully
		 * read, then the routine causes a fatal error.
		 * \param[in,out] vo the ordering class to use.
		 * \param[in] filename the name of the file to open and read
		 *                     from. */
		inline void import(particle_order & vo, const char* filename) {
			FILE* fp = safe_fopen(filename, "r");
import(vo,fp);
			fclose(fp);
		}
		void compute_all_cells();
		double sum_cell_volumes();
		/** Dumps particle IDs and positions to a file.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_particles(c_loop& vl, FILE* fp) {
			double* pp;
			if (vl.start()) do {
				pp = p[vl.ijk] + 3 * vl.q;
				fprintf(fp, "%d %g %g %g\n", id[vl.ijk][vl.q], *pp, pp[1], pp[2]);
			} while (vl.inc());
		}
		/** Dumps all of the particle IDs and positions to a file.
		 * \param[in] fp a file handle to write to. */
		inline void draw_particles(FILE* fp = stdout) {
			c_loop_all_periodic vl(*this);
			draw_particles(vl, fp);
		}
		/** Dumps all of the particle IDs and positions to a file.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_particles(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_particles(fp);
			fclose(fp);
		}
		/** Dumps particle positions in POV-Ray format.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_particles_pov(c_loop& vl, FILE* fp) {
			double* pp;
			if (vl.start()) do {
				pp = p[vl.ijk] + 3 * vl.q;
				fprintf(fp, "// id %d\nsphere{<%g,%g,%g>,s}\n",
					id[vl.ijk][vl.q], *pp, pp[1], pp[2]);
			} while (vl.inc());
		}
		/** Dumps all particle positions in POV-Ray format.
		 * \param[in] fp a file handle to write to. */
		inline void draw_particles_pov(FILE* fp = stdout) {
			c_loop_all_periodic vl(*this);
			draw_particles_pov(vl, fp);
		}
		/** Dumps all particle positions in POV-Ray format.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_particles_pov(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_particles_pov(fp);
			fclose(fp);
		}
		/** Computes Voronoi cells and saves the output in gnuplot
		 * format.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_cells_gnuplot(c_loop& vl, FILE* fp) {
			CHV_Cell c; double* pp;
			if (vl.start()) do if (compute_cell(c, vl)) {
				pp = p[vl.ijk] + ps * vl.q;
				c.draw_gnuplot(*pp, pp[1], pp[2], fp);
			} while (vl.inc());
		}
		/** Computes all Voronoi cells and saves the output in gnuplot
		 * format.
		 * \param[in] fp a file handle to write to. */
		inline void draw_cells_gnuplot(FILE* fp = stdout) {
			c_loop_all_periodic vl(*this);
			draw_cells_gnuplot(vl, fp);
		}
		/** Compute all Voronoi cells and saves the output in gnuplot
		 * format.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_cells_gnuplot(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_cells_gnuplot(fp);
			fclose(fp);
		}
		/** Computes Voronoi cells and saves the output in POV-Ray
		 * format.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_cells_pov(c_loop& vl, FILE* fp) {
			CHV_Cell c; double* pp;
			if (vl.start()) do if (compute_cell(c, vl)) {
				fprintf(fp, "// cell %d\n", id[vl.ijk][vl.q]);
				pp = p[vl.ijk] + ps * vl.q;
				c.draw_pov(*pp, pp[1], pp[2], fp);
			} while (vl.inc());
		}
		/** Computes all Voronoi cells and saves the output in POV-Ray
		 * format.
		 * \param[in] fp a file handle to write to. */
		inline void draw_cells_pov(FILE* fp = stdout) {
			c_loop_all_periodic vl(*this);
			draw_cells_pov(vl, fp);
		}
		/** Computes all Voronoi cells and saves the output in POV-Ray
		 * format.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_cells_pov(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_cells_pov(fp);
			fclose(fp);
		}
		/** Computes the Voronoi cells and saves customized information
		 * about them.
		 * \param[in] vl the loop class to use.
		 * \param[in] format the custom output string to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void print_custom(c_loop& vl, const char* format, FILE* fp) {
			int ijk, q; double* pp;
			if (contains_neighbor(format)) {
				CHV_Cell_Neighbor c;
				if (vl.start()) do if (compute_cell(c, vl)) {
					ijk = vl.ijk; q = vl.q; pp = p[ijk] + ps * q;
					c.output_custom(format, id[ijk][q], *pp, pp[1], pp[2], default_radius, fp);
				} while (vl.inc());
			}
			else {
				CHV_Cell c;
				if (vl.start()) do if (compute_cell(c, vl)) {
					ijk = vl.ijk; q = vl.q; pp = p[ijk] + ps * q;
					c.output_custom(format, id[ijk][q], *pp, pp[1], pp[2], default_radius, fp);
				} while (vl.inc());
			}
		}
		void print_custom(const char* format, FILE* fp = stdout);
		void print_custom(const char* format, const char* filename);
		bool find_voronoi_cell(double x, double y, double z, double& rx, double& ry, double& rz, int& pid);
		/** Computes the Voronoi cell for a particle currently being
		 * referenced by a loop class.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] vl the loop class to use.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed because it was removed entirely for some reason,
		 * then the routine returns false. */
		template<class v_cell, class c_loop>
		inline bool compute_cell(v_cell& c, c_loop& vl) {
			return vc.compute_cell(c, vl.ijk, vl.q, vl.i, vl.j, vl.k);
		}
		/** Computes the Voronoi cell for given particle.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] ijk the block that the particle is within.
		 * \param[in] q the index of the particle within the block.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed because it was removed entirely for some reason,
		 * then the routine returns false. */
		template<class v_cell>
		inline bool compute_cell(v_cell& c, int ijk, int q) {
			int k(ijk / (nx * oy)), ijkt(ijk - (nx * oy) * k), j(ijkt / nx), i(ijkt - j * nx);
			return vc.compute_cell(c, ijk, q, i, j, k);
		}
		/** Computes the Voronoi cell for a ghost particle at a given
		 * location.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] (x,y,z) the location of the ghost particle.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed, if it is removed entirely by a wall or boundary
		 * condition, then the routine returns false. */
		template<class v_cell>
		inline bool compute_ghost_cell(v_cell& c, double x, double y, double z) {
			int ijk;
			put_locate_block(ijk, x, y, z);
			double* pp = p[ijk] + 3 * co[ijk]++;
			*(pp++) = x; *(pp++) = y; *(pp++) = z;
			bool q = compute_cell(c, ijk, co[ijk] - 1);
			co[ijk]--;
			return q;
		}
	private:
		CHV_Compute<container_periodic> vc;
		friend class CHV_Compute<container_periodic>;
	};

	/** \brief Extension of the container_periodic_base class for computing radical
	 * Voronoi tessellations.
	 *
	 * This class is an extension of container_periodic_base that has routines
	 * specifically for computing the radical Voronoi tessellation that depends
	 * on the particle radii. */
	class HWIVORO_DLL container_periodic_poly : public container_periodic_base, public radius_poly {
	public:
		container_periodic_poly(double bx_, double bxy_, double by_, double bxz_, double byz_, double bz_,
			int nx_, int ny_, int nz_, int init_mem_);
		void clear();
		void put(int n, double x, double y, double z, double r);
		void put(int n, double x, double y, double z, double r, int& ai, int& aj, int& ak);
		void put(particle_order& vo, int n, double x, double y, double z, double r);
		void import(FILE * fp = stdin);
		void import(particle_order & vo, FILE * fp = stdin);
		/** Imports a list of particles from an open file stream into
		 * the container_poly class. Entries of five numbers (Particle
		 * ID, x position, y position, z position, radius) are searched
		 * for. If the file cannot be successfully read, then the
		 * routine causes a fatal error.
		 * \param[in] filename the name of the file to open and read
		 *                     from. */
		inline void import(const char* filename) {
			FILE* fp = safe_fopen(filename, "r");
import(fp);
			fclose(fp);
		}
		/** Imports a list of particles from an open file stream into
		 * the container_poly class. Entries of five numbers (Particle
		 * ID, x position, y position, z position, radius) are searched
		 * for. In addition, the order in which particles are read is
		 * saved into an ordering class. If the file cannot be
		 * successfully read, then the routine causes a fatal error.
		 * \param[in,out] vo the ordering class to use.
		 * \param[in] filename the name of the file to open and read
		 *                     from. */
		inline void import(particle_order & vo, const char* filename) {
			FILE* fp = safe_fopen(filename, "r");
import(vo,fp);
			fclose(fp);
		}
		void compute_all_cells();
		double sum_cell_volumes();
		/** Dumps particle IDs, positions and radii to a file.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_particles(c_loop& vl, FILE* fp) {
			double* pp;
			if (vl.start()) do {
				pp = p[vl.ijk] + 4 * vl.q;
				fprintf(fp, "%d %g %g %g %g\n", id[vl.ijk][vl.q], *pp, pp[1], pp[2], pp[3]);
			} while (vl.inc());
		}
		/** Dumps all of the particle IDs, positions and radii to a
		 * file.
		 * \param[in] fp a file handle to write to. */
		inline void draw_particles(FILE* fp = stdout) {
			c_loop_all_periodic vl(*this);
			draw_particles(vl, fp);
		}
		/** Dumps all of the particle IDs, positions and radii to a
		 * file.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_particles(const char* filename) {
			FILE* fp = safe_fopen(filename, "w");
			draw_particles(fp);
			fclose(fp);
		}
		/** Dumps particle positions in POV-Ray format.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_particles_pov(c_loop& vl, FILE* fp) {
			double* pp;
			if (vl.start()) do {
				pp = p[vl.ijk] + 4 * vl.q;
				fprintf(fp, "// id %d\nsphere{<%g,%g,%g>,%g}\n",
					id[vl.ijk][vl.q], *pp, pp[1], pp[2], pp[3]);
			} while (vl.inc());
		}
		/** Dumps all the particle positions in POV-Ray format.
		 * \param[in] fp a file handle to write to. */
		inline void draw_particles_pov(FILE* fp = stdout) {
			c_loop_all_periodic vl(*this);
			draw_particles_pov(vl, fp);
		}
		/** Dumps all the particle positions in POV-Ray format.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_particles_pov(const char* filename) {
			FILE* fp(safe_fopen(filename, "w"));
			draw_particles_pov(fp);
			fclose(fp);
		}
		/** Computes Voronoi cells and saves the output in gnuplot
		 * format.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_cells_gnuplot(c_loop& vl, FILE* fp) {
			CHV_Cell c; double* pp;
			if (vl.start()) do if (compute_cell(c, vl)) {
				pp = p[vl.ijk] + ps * vl.q;
				c.draw_gnuplot(*pp, pp[1], pp[2], fp);
			} while (vl.inc());
		}
		/** Compute all Voronoi cells and saves the output in gnuplot
		 * format.
		 * \param[in] fp a file handle to write to. */
		inline void draw_cells_gnuplot(FILE* fp = stdout) {
			c_loop_all_periodic vl(*this);
			draw_cells_gnuplot(vl, fp);
		}
		/** Compute all Voronoi cells and saves the output in gnuplot
		 * format.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_cells_gnuplot(const char* filename) {
			FILE* fp(safe_fopen(filename, "w"));
			draw_cells_gnuplot(fp);
			fclose(fp);
		}
		/** Computes Voronoi cells and saves the output in POV-Ray
		 * format.
		 * \param[in] vl the loop class to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void draw_cells_pov(c_loop& vl, FILE* fp) {
			CHV_Cell c; double* pp;
			if (vl.start()) do if (compute_cell(c, vl)) {
				fprintf(fp, "// cell %d\n", id[vl.ijk][vl.q]);
				pp = p[vl.ijk] + ps * vl.q;
				c.draw_pov(*pp, pp[1], pp[2], fp);
			} while (vl.inc());
		}
		/** Computes all Voronoi cells and saves the output in POV-Ray
		 * format.
		 * \param[in] fp a file handle to write to. */
		inline void draw_cells_pov(FILE* fp = stdout) {
			c_loop_all_periodic vl(*this);
			draw_cells_pov(vl, fp);
		}
		/** Computes all Voronoi cells and saves the output in POV-Ray
		 * format.
		 * \param[in] filename the name of the file to write to. */
		inline void draw_cells_pov(const char* filename) {
			FILE* fp(safe_fopen(filename, "w"));
			draw_cells_pov(fp);
			fclose(fp);
		}
		/** Computes the Voronoi cells and saves customized information
		 * about them.
		 * \param[in] vl the loop class to use.
		 * \param[in] format the custom output string to use.
		 * \param[in] fp a file handle to write to. */
		template<class c_loop>
		void print_custom(c_loop& vl, const char* format, FILE* fp) {
			int ijk, q; double* pp;
			if (contains_neighbor(format)) {
				CHV_Cell_Neighbor c;
				if (vl.start()) do if (compute_cell(c, vl)) {
					ijk = vl.ijk; q = vl.q; pp = p[ijk] + ps * q;
					c.output_custom(format, id[ijk][q], *pp, pp[1], pp[2], pp[3], fp);
				} while (vl.inc());
			}
			else {
				CHV_Cell c;
				if (vl.start()) do if (compute_cell(c, vl)) {
					ijk = vl.ijk; q = vl.q; pp = p[ijk] + ps * q;
					c.output_custom(format, id[ijk][q], *pp, pp[1], pp[2], pp[3], fp);
				} while (vl.inc());
			}
		}
		/** Computes the Voronoi cell for a particle currently being
		 * referenced by a loop class.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] vl the loop class to use.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed because it was removed entirely for some reason,
		 * then the routine returns false. */
		template<class v_cell, class c_loop>
		inline bool compute_cell(v_cell& c, c_loop& vl) {
			return vc.compute_cell(c, vl.ijk, vl.q, vl.i, vl.j, vl.k);
		}
		/** Computes the Voronoi cell for given particle.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] ijk the block that the particle is within.
		 * \param[in] q the index of the particle within the block.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed because it was removed entirely for some reason,
		 * then the routine returns false. */
		template<class v_cell>
		inline bool compute_cell(v_cell& c, int ijk, int q) {
			int k(ijk / (nx * oy)), ijkt(ijk - (nx * oy) * k), j(ijkt / nx), i(ijkt - j * nx);
			return vc.compute_cell(c, ijk, q, i, j, k);
		}
		/** Computes the Voronoi cell for a ghost particle at a given
		 * location.
		 * \param[out] c a Voronoi cell class in which to store the
		 * 		 computed cell.
		 * \param[in] (x,y,z) the location of the ghost particle.
		 * \param[in] r the radius of the ghost particle.
		 * \return True if the cell was computed. If the cell cannot be
		 * computed, if it is removed entirely by a wall or boundary
		 * condition, then the routine returns false. */
		template<class v_cell>
		inline bool compute_ghost_cell(v_cell& c, double x, double y, double z, double r) {
			int ijk;
			put_locate_block(ijk, x, y, z);
			double* pp = p[ijk] + 4 * co[ijk]++, tm = max_radius;
			*(pp++) = x; *(pp++) = y; *(pp++) = z; *pp = r;
			if (r > max_radius) max_radius = r;
			bool q = compute_cell(c, ijk, co[ijk] - 1);
			co[ijk]--; max_radius = tm;
			return q;
		}
		void print_custom(const char* format, FILE* fp = stdout);
		void print_custom(const char* format, const char* filename);
		bool find_voronoi_cell(double x, double y, double z, double& rx, double& ry, double& rz, int& pid);
	private:
		CHV_Compute<container_periodic_poly> vc;
		friend class CHV_Compute<container_periodic_poly>;
	};

}

