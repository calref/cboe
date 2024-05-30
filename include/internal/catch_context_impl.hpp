/*
 *  Created by Phil on 31/12/2010.
 *  Copyright 2010 Two Blue Cubes Ltd. All rights reserved.
 *
 *  Distributed under the Boost Software License, Version 1.0. (See accompanying
 *  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
 */
#ifndef TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED
#define TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED

#include "catch_runner_impl.hpp"

#include "catch_context.h"
#include "catch_stream.hpp"

namespace Catch {

    class Context : public IMutableContext {

        Context() : m_config( NULL ), m_runner( NULL ), m_resultCapture( NULL ) {}
        Context( Context const& );
        void operator=( Context const& );

    public: // IContext
        virtual IResultCapture* getResultCapture() {
            return m_resultCapture;
        }
        virtual IRunner* getRunner() {
            return m_runner;
        }
        virtual size_t getGeneratorIndex( std::string const& fileInfo, size_t totalSize ) {
            return getGeneratorsForCurrentTest()
            .getGeneratorInfo( fileInfo, totalSize )
            .getCurrentIndex();
        }
        virtual bool advanceGeneratorsForCurrentTest() {
            IGeneratorsForTest* generators = findGeneratorsForCurrentTest();
            return generators && generators->moveNext();
        }

        virtual Ptr<IConfig const> getConfig() const {
            return m_config;
        }

    public: // IMutableContext
        virtual void setResultCapture( IResultCapture* resultCapture ) {
            m_resultCapture = resultCapture;
        }
        virtual void setRunner( IRunner* runner ) {
            m_runner = runner;
        }
        virtual void setConfig( Ptr<IConfig const> const& config ) {
            m_config = config;
        }

        friend IMutableContext& getCurrentMutableContext();

    private:
        IGeneratorsForTest* findGeneratorsForCurrentTest() {
            std::string testName = getResultCapture()->getCurrentTestName();

            std::map<std::string, IGeneratorsForTest*>::const_iterator it =
                m_generatorsByTestName.find( testName );
            return it != m_generatorsByTestName.end()
                ? it->second
                : NULL;
        }

        IGeneratorsForTest& getGeneratorsForCurrentTest() {
            IGeneratorsForTest* generators = findGeneratorsForCurrentTest();
            if( !generators ) {
                std::string testName = getResultCapture()->getCurrentTestName();
                generators = createGeneratorsForTest();
                m_generatorsByTestName.insert( std::make_pair( testName, generators ) );
            }
            return *generators;
        }

    private:
        Ptr<IConfig const> m_config;
        IRunner* m_runner;
        IResultCapture* m_resultCapture;
        std::map<std::string, IGeneratorsForTest*> m_generatorsByTestName;
    };

    namespace {
        Context* currentContext = NULL;
    }
    IMutableContext& getCurrentMutableContext() {
        if( !currentContext )
            currentContext = new Context();
        return *currentContext;
    }
    IContext& getCurrentContext() {
        return getCurrentMutableContext();
    }

    Stream createStream( std::string const& streamName ) {
        if( streamName == "stdout" ) return Stream( Catch::cout().rdbuf(), false );
        if( streamName == "stderr" ) return Stream( Catch::cerr().rdbuf(), false );
        if( streamName == "debug" ) return Stream( new StreamBufImpl<OutputDebugWriter>, true );

        throw std::domain_error( "Unknown stream: " + streamName );
    }

    void cleanUpContext() {
        delete currentContext;
        currentContext = NULL;
    }
}

#endif // TWOBLUECUBES_CATCH_CONTEXT_IMPL_HPP_INCLUDED
