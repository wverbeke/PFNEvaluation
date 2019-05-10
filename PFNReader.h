#ifndef PFNReader_H
#define PFNReader_H

//include std classes
#include <string>
#include <utility>


//libraries to interface c++ with python
#include <boost/python.hpp>


class PFNReader{
    public:
        PFNReader(const std::string&, const std::pair< unsigned, unsigned >&, const unsigned);
        PFNReader( const PFNReader& );
        ~PFNReader();

        PFNReader& operator=( const PFNReader& ) = default;

        double predict( const std::vector< std::vector< double > >&, const std::vector< double >& ) const;

    private:
        
        //input shape of the particle-flow information {number_of_particles, number_of_features}
        std::pair< unsigned, unsigned > pfnShape;

        //number of high-level features
        unsigned highlevelShape;

        //instance counter
        static unsigned instanceCounter;

        static boost::python::object pythonModule;
        boost::python::object kerasModel;
        boost::python::object predictRoutine;

        //set up python environment
        static void initializePythonAPI();
        
        //load python module 
        static void loadPythonModule( const std::string& );

        //load keras model
        void loadKerasModel( const std::string& );

        //check input shapes 
        bool checkPfnShape( const std::vector< std::vector< double > >& ) const;
        bool checkHighlevelShape( const std::vector< double >& ) const;
};

#endif
